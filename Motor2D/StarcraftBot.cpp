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
	units.add(App->AI->CreateEnemyBot(x, y, unit_2, this));
}

StarcraftBot::~StarcraftBot()
{
	units.clear();
	endangeredUnits.clear();
}

bool StarcraftBot::Update(float dt)
{
	bool ret = true;

	if (units.count() < maxPopulation)
	{
		//TODO 5: Create Units

		if (SpawnTime <= spawnTimer.ReadSec())
		{
			int x = base.x;
			int y = base.y;
			units.add(App->AI->CreateEnemyBot(x, y, unit_2));
			spawnTimer.Start();
		}


		
	}
	else // max population: lauch attack
	{
		//TODO 6: fill in what the units must do when the starcraft bot is loaded

		C_List<Bot*> enemies;

		if (App->AI->GetEnemies(team, &enemies))
		{
			C_List_item<Bot*>* bot = idleUnits.start;
			while (bot)
			{
				if (bot->data->target != NULL)
					bot->data->Attack(enemies.start->data);

				bot = bot->next;
			}
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
	C_List<Bot*> idleUnits;
	C_List_item<Bot*>* bot = NULL;
	Bot* ally = NULL;

	for (C_List_item<Bot*>* unit = endangeredUnits.start; unit; unit = unit->next)
	{
		for (bot = units.start; bot; bot = bot->next)
		{
			if (bot->data->state == idle)
				idleUnits.add(bot->data);
		}
		
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
	endangeredUnits.del(endangeredUnits.At(endangeredUnits.find(unit)));
	idleUnits.del(idleUnits.At(idleUnits.find(unit)));
}


void StarcraftBot::OnUnitDanger(Bot* unit)
{
	if (endangeredUnits.find(unit) != -1)
	{
		endangeredUnits.add(unit);
		if (!attacking)
			unit->GoTo(base);
	}
}