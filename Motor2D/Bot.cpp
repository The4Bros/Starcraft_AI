#include "Bot.h"
#include "j1App.h"
#include "M_EntityManager.h"
#include "M_AI.h"
#include "Unit.h"
#include "M_PathFinding.h"
#include "M_Map.h"

Bot::Bot()
{

}
Bot::Bot(int x, int y, Unit_Type type, float team, StarcraftBot* father)
{
	unit = App->entityManager->CreateUnit(x, y, type, team);
	initialPos.x = x;
	initialPos.y = y;
	target = NULL;
	this->father = father;
	updateTimer.Start();
	attackTimer.Start();
}

Bot::~Bot()
{

}

 bool Bot::Update(float dt)
{
	if (unit->GetHP() <= 0)
		return false;

	bool ret = true;

	if (fixedUpdateSeconds <= updateTimer.ReadSec())
	{
		FixedUpdate();
		updateTimer.Start();
	}

	if (!father)
	{
		//has to handle own behaviour

		if (state == attack)
		{
			if (unit->isTargetReached())
			{
				if (attackSpeed <= attackTimer.ReadSec())
				{
					target->OnAttack(damage, this);
					attackTimer.Start();
				}
			}
		}
	}

	return ret;
}

bool Bot::FixedUpdate()
{
	bool ret = true;
	switch (state)
	{
 	case BotState::idle:
		LOG("IDLE STATE");
		if (CheckForEnemies())
		{
			SetState(attack);
		}
		break;

	case BotState::attack:
		LOG("ATTACK STATE");
		if (target)
		{
			if (flees)
			{
				if (unit->GetHP() < unit->GetMaxHP() / 10) //flee when less than 10% health
					SetState(flee);
			}
			if (kites && unit->isTargetReached())
			{
				float distance = DistanceBetweenUnits(target->unit, unit);

				if (distance < attackRange * 0.3f)
				{
					int x, y;

					x = unit->GetPosition().x;
					y = unit->GetPosition().y;

					/******************************
					SET X & Y FOR KITING POSITION
					*******************************/

					C_DynArray<iPoint> newPath;
					fPoint unitPos = unit->GetPosition();
					iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));
					App->pathFinding->GetNewPath(unitTile, { x, y }, newPath);
					unit->SetNewPath(newPath);
				}

			}
			else if (!EnemyOnRange(target->unit, unit, attackRange))
			{
				C_DynArray<iPoint> newPath;
				fPoint unitPos = unit->GetPosition();
				fPoint targetPos = target->unit->GetPosition();
				iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));
				iPoint enemyTile = App->map->WorldToMap(round(targetPos.x), round(targetPos.y));
				App->pathFinding->GetNewPath(unitTile, enemyTile, newPath);
				unit->SetNewPath(newPath);
			}
		}
		else
		{
			SetState(idle);
		}

		break;

	case BotState::flee:
		LOG("FLEE STATE");
		if (unit->isTargetReached())
		{
			if (DistanceBetweenUnits(target->unit, unit) < sightRange * 4.0f)
			{ 
				// flees in random position
				int x = (rand() % 600) + unit->GetPosition().x;
				int y = (rand() % 600) + unit->GetPosition().y;

				C_DynArray<iPoint> newPath;
				fPoint unitPos = unit->GetPosition();
				iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));
				App->pathFinding->GetNewPath(unitTile, { x, y }, newPath);
				unit->SetNewPath(newPath);
			}
			else
			{
				SetState(idle);
			}
		}
		
		break;
	}
	return ret;
}





bool Bot::CheckForEnemies()
{
	bool ret = false;
	C_List<Bot*> potentialTargets;
	C_List_item<Bot*>* item = App->AI->botList.start;

	// check for potential enemy bots
	while (item)
	{
		if (item->data->GetTeam() != unit->team)
		{
			if (DistanceBetweenUnits(item->data->unit, unit) <= sightRange)
				potentialTargets.add(item->data);
		}

		item = item->next;
	}

	if (potentialTargets.count() > 0)
	{
		item = App->AI->botList.start;
		
		float closestdistance = DistanceBetweenUnits(item->data->unit, unit);
		float distance = 0.0f;

		//filter the list
		while (item)
		{
			distance = DistanceBetweenUnits(item->data->unit, unit);

			if (distance <= closestdistance)
			{
				closestdistance = distance;
				target = item->data;
			}

			item = item->next;
		}

		SetState(attack);
		ret = true;
	}

	return ret;
}

void Bot::FollowTarget()
{
	unit->SetTarget(target->unit->GetPosition().x - (target->unit->colRadius + unit->colRadius), target->unit->GetPosition().y - (target->unit->colRadius + unit->colRadius));
}

bool Bot::KiteFromEnemy(int attackrange)
{
	C_Vec2<float> distance = { target->unit->GetPosition().x - unit->GetPosition().x, target->unit->GetPosition().y - unit->GetPosition().y };
	if (distance.x > 0 && distance.y > 0)
	{
		unit->SetTarget(target->unit->GetPosition().x + (target->unit->colRadius + attackrange), target->unit->GetPosition().y - (target->unit->colRadius + attackrange));
	}
	else if (distance.x > 0 && distance.y < 0)
	{
		unit->SetTarget(target->unit->GetPosition().x + (target->unit->colRadius + attackrange), target->unit->GetPosition().y + (target->unit->colRadius + attackrange));
	}
	else if (distance.x < 0 && distance.y < 0)
	{
		unit->SetTarget(target->unit->GetPosition().x - (target->unit->colRadius + attackrange), target->unit->GetPosition().y + (target->unit->colRadius + attackrange));
		
	}
	else
	{
		unit->SetTarget(target->unit->GetPosition().x - (target->unit->colRadius + attackrange), target->unit->GetPosition().y - (target->unit->colRadius + attackrange));
	}

	return true;
}


void Bot::FindPlaceToFight(){

	C_Vec2<float> distance = { (target->unit->GetPosition().x + unit->GetPosition().x) / 2, (target->unit->GetPosition().y + unit->GetPosition().y) / 2 };
	
	if (App->pathFinding->IsWalkable(distance.x , distance.y))
		unit->SetTarget(distance.x - (target->unit->colRadius + 20), distance.y - (target->unit->colRadius + 20));
	
}

bool Bot::EnemyOnRange(Unit* unit1, Unit* unit2, float range)
{
	C_Vec2<float> distance = { unit1->GetPosition().x - unit2->GetPosition().x, unit1->GetPosition().y - unit2->GetPosition().y };
	return (distance.GetModule() < range + unit1->colRadius + unit2->colRadius);
}

float Bot::DistanceBetweenUnits(Unit* unit1, Unit* unit2)
{
	C_Vec2<float> distance = { unit1->GetPosition().x - unit2->GetPosition().x, unit1->GetPosition().y - unit2->GetPosition().y };
	return distance.GetModule();
}

C_Vec2<float> Bot::DistanceWithTarget()
{
	C_Vec2<float> distance = { unit->GetPosition().x - target->unit->GetPosition().x, unit->GetPosition().y - target->unit->GetPosition().y };
	 return distance;
}

fPoint Bot::GetPos()
{
	return unit->GetPosition();
}

float Bot::GetTeam()
{
	float ret = 0.0f;

	if (unit)
		ret = unit->team;

	return ret;
}

bool Bot::SetStats(std::pair<const char*, std::map<const char*, SimpleCVar >> entity_stats, int index)
{
	bool ret = true;
	std::map<const char*, SimpleCVar>::iterator it = entity_stats.second.begin();

	for (int i = 0; i < index; i++)
		it++;

	if (!(ret = it->second.Read(&damage))) return ret;

	it++;
	if (!(ret = it->second.Read(&sightRange))) return ret;

	it++;
	if (!(ret = it->second.Read(&attackRange))) return ret;

	it++;
	if (!(ret = it->second.Read(&attackSpeed))) return ret;

	it++;
	if (!(ret = it->second.Read(&kites))) return ret;

	it++;
	if (!(ret = it->second.Read(&flees))) return ret;

	return ret;
}

void Bot::SetState(BotState bState)
{
	state = bState;
	switch (state)
	{
	case idle:
		App->AI->OnUnitIdle(father, this);
		break;
	}
}



void Bot::OnAttack(int damage, Bot* attacker)
{
	unit->Hit(damage, normal);

	if (unit->GetHP() <= 0)
	{
		App->entityManager->deleteUnit(unit);
		App->AI->deadBotList.add(this);

		if (father)
			App->AI->OnUnitKill(father, this);
	}
	else if (father)
	{
		App->AI->OnUnitDanger(father, this);
	}
	else if (flees)
	{
		if (unit->GetHP() < unit->GetMaxHP() / 10)
			SetState(flee);
		else
			SetState(attack);
	}
	else
	{
		SetState(attack);
	}
}

void Bot::Attack(Bot* target)
{
	if (target != NULL)
	{
		SetState(attack);
		this->target = target;
	}
}


void Bot::GoTo(fPoint pos)
{
	App->entityManager->SendNewPath(pos.x, pos.y, unit);
}