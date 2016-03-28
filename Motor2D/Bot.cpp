#include "Bot.h"
#include "j1App.h"
#include "M_EntityManager.h"
#include "M_AI.h"

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

bool Bot::Update()
{
	bool ret = true;

	if (fixedUpdateSeconds <= updateTimer.ReadSec())
	{
		FixedUpdate();
		updateTimer.Start();
	}
		

	switch (state)
	{
	case BotState::idle:

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

