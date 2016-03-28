#include "Bot.h"
#include "j1App.h"
#include "M_EntityManager.h"
#include "M_AI.h"
#include "Unit.h"

Bot::Bot()
{

}
Bot::Bot(int x, int y, Unit_Type type, float team)
{
	unit = App->entityManager->CreateUnit(x, y, type, team);
	target = NULL;
	targetLastPos.SetToZero();
	updateTimer.Start();
}

Bot::~Bot()
{

}

 bool Bot::Update(float dt)
{
	bool ret = true;

	if (fixedUpdateSeconds <= updateTimer.ReadSec())
	{
		FixedUpdate();
		updateTimer.Start();
	}
	if (unit){
		unit->SetTarget(0, 0);
	}
 
	switch (state)
	{
	case BotState::idle:
		CheckForEnemies();

		break;

	case BotState::attack:

		break;

	case BotState::kite:

		break;

	case BotState::flee:

		break;
	}

	return ret;
}

bool Bot::FixedUpdate()
{
	bool ret = true;

	return ret;
}

bool PostUpdate(float dt);

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
	if (!(ret = it->second.Read(&kites))) return ret;

	it++;
	if (!(ret = it->second.Read(&flees))) return ret;

	return ret;
}

fPoint Bot::GetPos()
{
	return unit->GetPosition();
}


void Bot::OnAttack(int damage, Bot* attacker)
{
	unit->Damage(damage, normal);

	if (unit->GetHP() <= 0)
	{
		App->entityManager->deleteUnit(unit);
		App->AI->deadBotList.add(this);
	}
}

bool Bot::CheckForEnemies(){
	/*
	C_List_item<Unit*>* item = NULL;
	item = App->entityManager->unitList.start;
	C_List<Unit*> OtherTeamList;


	while (item)
	{
		if (item->data->team != unit->team)
			OtherTeamList.add(item->data);

		item = item->next;
	}

	C_List_item<Unit*>* item2 = NULL;
	item2 = OtherTeamList.start;

	while (item2)
	{
		if (EnemyOnUnitRange(item2->data, unit))
		{
			target = item2->data;
			return true;
		}

		item = item->next;
	}

	*/

	return false;
}


bool Bot::EnemyOnUnitRange(Unit* unit1, Unit* unit2)
{
	C_Vec2<float> distance = { unit1->GetPosition().x - unit2->GetPosition().x, unit1->GetPosition().y - unit2->GetPosition().y };
	return (distance.GetModule() < 100);
}

void Bot::SetState(BotState newstate)
{
	state = newstate;
}