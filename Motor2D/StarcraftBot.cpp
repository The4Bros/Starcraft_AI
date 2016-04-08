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
	units.clear();
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

	if (units.count() == 0) // defeated
	{
		ret = false;
	}
	else if (units.count() < maxPopulation) // get stronger
	{
		if (SpawnTime <= spawnTimer.ReadSec())
		{
			//TODO 5: Create Units
			int x = base.x;
			int y = base.y;
			units.add(App->AI->CreateEnemyBot(x, y, unit_2, this));
			spawnTimer.Start();
		}
	}
	else// max population: lauch attack
	{
		//TODO 6: fill in what the units must do when the starcraft bot is loaded

		C_List<Bot*> enemies;

		if (App->AI->GetEnemies(team, &enemies))
		{
			C_List_item<Bot*>* bot = units.start;
			while (bot)
			{
				if (bot->data->target == NULL || App->entityManager->unitList.find(bot->data->target->unit) == -1)
				{
					bot->data->Attack(enemies.start->data);
				}

				bot = bot->next;
			}
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
		endangeredUnits.add(unit);

	idleUnits.del(idleUnits.At(idleUnits.find(unit)));
}

void StarcraftBot::OnUnitIdle(Bot* unit)
{
	if (idleUnits.find(unit) != -1)
		idleUnits.add(unit);

	endangeredUnits.del(endangeredUnits.At(endangeredUnits.find(unit)));
}