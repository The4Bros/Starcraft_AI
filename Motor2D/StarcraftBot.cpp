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
			units.add(new Bot(x, y, unit_3, team, this));
			spawnTimer.Start();
		}
	}
	else
	{
		
		C_List_item<Bot*>* bot = units.start;

		while (bot)
		{
			if (bot->data->target != NULL)


			bot = bot->next;
		}
	}


	if (fixedUpdateSeconds <= updateTimer.ReadSec())
	{
		FixedUpdate();
		updateTimer.Start();
	}

	if (units.count() > 0)
	{
		C_List_item<Bot*>* bot = NULL;
		bot = units.start;

		while (bot)
		{



			bot = bot->next;
		}
	}

	return ret;
}

bool StarcraftBot::FixedUpdate()
{
	bool ret = true;

	if (endangeredUnits.count() > 0)
	{
		C_List<Bot*> availableUnits;

		for (C_List_item<Bot*>* bot = units.start; bot; bot = bot->next)
		{
			if (bot->data->state == idle)
				availableUnits.add(bot->data);
		}

		if (availableUnits.count() > 0)
		{
			C_List_item<Bot*>* endangered = endangeredUnits.start;

			// repartir ataque



		}
		
		int unitRatio =  endangeredUnits.count();
	}

	enemies = App->AI->GetEnemies(team);

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