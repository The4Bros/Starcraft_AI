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
	unit = App->entityManager->CreateUnit(x, y, type, team, this);
	initialPos.x = x;
	initialPos.y = y;
	target = NULL;
	this->father = father;
	updateTimer.Start();
	attackTimer.Start();
}

Bot::~Bot()
{
	App->entityManager->deleteUnit(unit);
	target = NULL;
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

	return ret;
}

bool Bot::FixedUpdate()
{
	bool ret = true;
	switch (state)
	{
 	case BotState::idle:
		LOG("IDLE STATE");

		// Check if the Unit is a player Unit
		if (unit->team == App->AI->playerTeam)
		{
			if (unit->ArrivedToDestination())
				CheckForEnemies();
		}

		else
		{
			CheckForEnemies();
		}
		break;

	case BotState::attack:
		LOG("ATTACK STATE");
		if (App->AI->botList.find(target) != -1)
		{
			//If this Unit Evades when low life, Set the state to flee
			if (flees)
			{
				if (unit->GetHP() < unit->GetMaxHP() / 4) //flee when less than 10% health
				{
						SetState(flee);
						break;
				}
			}
			//Check if target is on the Attack Range
			if (EnemyOnRange(target->unit, unit, attackRange))
			{
				//TODO: 3 - Attack target
				if (!unit->ArrivedToDestination())
					unit->Stop();
				//Attack Unit 
				//Attack according to attackspeed
				if (attackTimer.ReadSec() >= attackSpeed)
				{
					attackTimer.Start();
					target->OnAttack(damage, this);
				}
			}
			//If is not on the attack range, chase him
			else if (unit->team == App->AI->playerTeam || father != NULL || EnemyOnRange(target->unit, unit, sightRange))
			{
				//TODO: 2 - Chase target

				// Compare target position with unit position 
				if (targetPos != target->GetPos() || unit->isTargetReached())
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
		}
		else
		{
			SetState(idle);
		}

		break;

	case BotState::flee:
		LOG("FLEE STATE");
		if (unit->ArrivedToDestination())
		{
			if (DistanceBetweenUnits(target->unit, unit) < sightRange * 1.5f)
			{ 
				// flees in random direction
				int x = (rand() % 60) + unit->GetPosition().x;
				int y = (rand() % 60) + unit->GetPosition().y;

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

	//TODO: 2 - Find target
	// Add to "potentialTargets"  list the Units of the other team that are on sightrange
	while (item)
	{
		if (item->data->GetTeam() != unit->team)
		{
			if (DistanceBetweenUnits(item->data->unit, unit) <= sightRange)
				potentialTargets.add(item->data);
		}

		item = item->next;
	}

	//TODO: 2 - Find target
	//Now let's find the closest one
	if (potentialTargets.count() > 0)
	{
		item = potentialTargets.start;
		
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
		//If we find a valid target Set the state to attack State
		SetState(attack);
		ret = true;
	}

	return ret;
}

// Useful Distance functions
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

void Bot::SetState(BotState _state)
{
	state = _state;
	switch (state)
	{
	case idle:
		target = NULL;
		App->AI->OnUnitIdle(father, this);
		break;
	}
}



void Bot::OnAttack(int damage, Bot* attacker)
{
	unit->Hit(damage, normal);

	if (unit->GetHP() <= 0)
	{
		OnKill();
	}
	else if (unit->team == App->AI->playerTeam)
	{
		if (unit->ArrivedToDestination())
		{
			Attack(attacker);
		}
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
	else if (state != attack)
		Attack(attacker);
}

void Bot::OnKill()
{
	App->AI->deadBotList.add(this);
}


void Bot::Attack(Bot* target)
{
	if (target != NULL)
	{
		SetState(attack);
		this->target = target;
		targetPos = target->GetPos();
	}
}


void Bot::GoTo(fPoint pos)
{
	App->entityManager->SendNewPath(pos.x, pos.y, unit);
}