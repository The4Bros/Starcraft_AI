#include "StarcraftBot.h"
#include "j1App.h"
#include "M_EntityManager.h"
#include "M_AI.h"
#include "Unit.h"
#include "Bot.h"


StarcraftBot::StarcraftBot()
{

}
StarcraftBot::StarcraftBot(int x, int y, float team)
{
	base.x = x;
	base.y = y;
	this->team = team;
	units.add(App->AI->CreateBot(x, y, unit_2, team));
}

StarcraftBot::~StarcraftBot()
{
	units.clear();
	endangeredUnits.clear();
	idleUnits.clear();
	enemies.clear();
}

bool StarcraftBot::Update(float dt)
{
	bool ret = true;

	if (units.count() < maxPopulation)
	{
		if (SpawnTime <= spawnTimer.ReadSec())
		{
			int x = base.x;
			int y = base.y;
			units.add(App->AI->CreateBot(x, y, unit_2, team));
			spawnTimer.Start();
		}
	}
	else // max population: lauch attack
	{
		C_List_item<Bot*>* bot = units.start;

		while (bot)
		{
			if (bot->data->target != NULL)
				bot->data->Attack(enemies.start->data);

			bot = bot->next;
		}
	}


	if (fixedUpdateSeconds <= updateTimer.ReadSec())
	{
		FixedUpdate();
		updateTimer.Start();
	}

	return ret;
}

bool StarcraftBot::FixedUpdate()
{
	bool ret = true;

	C_List_item<Bot*>* bot = NULL;
	Bot* ally = NULL;

	for (C_List_item<Bot*>* unit = endangeredUnits.start; unit; unit = unit->next)
	{
		if (idleUnits.count() > 0)
		{
			bot = idleUnits.start;
			ally = bot->data;

			int distance = bot->data->DistanceBetweenUnits(bot->data->unit, unit->data->unit);

			for (; bot; bot = bot->next)
			{
				if (bot->data->DistanceBetweenUnits(bot->data->unit, unit->data->unit) < distance)
					ally = bot->data;
			}

			ally->Attack(unit->data->target);
		}
	}

	return ret;
}

void StarcraftBot::OnUnitKill(Bot* unit)
{
	units.del(units.At(units.find(unit)));
}


void StarcraftBot::OnUnitDanger(Bot* unit)
{
	endangeredUnits.add(unit);
	if (!attacking)
		unit->GoTo(base);
}