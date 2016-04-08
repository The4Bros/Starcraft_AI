
#include "M_AI.h"

#include "j1App.h"
#include "M_Input.h"
#include "M_Textures.h"
//#include "M_Audio.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include "M_GUI.h"
#include "M_EntityManager.h"
#include "Entity.h"
#include "Unit.h"
//#include "j1Gui.h"
//#include "UIElements.h"
//#include "M_Fonts.h"
#include "M_Console.h"



M_AI::M_AI(bool start_enabled) : j1Module(start_enabled)
{
	name.create("AI");
}


M_AI::~M_AI()
{}

bool M_AI::PreStart(pugi::xml_node& node)
{
	std::pair<const char*, std::map<const char*, std::map<const char*, SimpleCVar>>> ly3_pair;
	std::pair<const char*, std::map<const char*, SimpleCVar>> ly2_pair;
	std::pair<const char*, SimpleCVar> ly1_pair;

	for (pugi::xml_node camp = node.first_child(); camp; camp = camp.next_sibling())
	{
		//camp name
		ly3_pair.first = camp.attribute("name").as_string(" ");

		for (pugi::xml_node entity = camp.first_child(); entity; entity = entity.next_sibling())
		{
			//entity name
			ly2_pair.first = entity.attribute("name").as_string(" ");

			//entity stats
			for (pugi::xml_node stat = entity.first_child(); stat; stat = stat.next_sibling())
			{
				//stat name
				ly1_pair.first = stat.child_value();

				//stat value
				switch (stat.attribute("type").as_int(0))
				{
				case 0:	ly1_pair.second = SimpleCVar(stat.attribute("value").as_float(0.0f));	break;
				case 1: ly1_pair.second = SimpleCVar(stat.attribute("value").as_int(0));		break;
				case 2: ly1_pair.second = SimpleCVar(stat.attribute("value").as_string(""));	break;
				case 3: ly1_pair.second = SimpleCVar(stat.attribute("value").as_bool(false));	break;
				}

				//add stat to entity
				ly2_pair.second.insert(ly1_pair);
			}

			//add entity
			ly3_pair.second.insert(ly2_pair);
		}

		//add camp to data
		data.insert(ly3_pair);
	}

	return true;
}
bool M_AI::Update(float dt)
{
	if (botList.count() > 0)
	{
		C_List_item<Bot*>* bot = NULL;
		bot = botList.start;

		while (bot)
		{
			if (!bot->data->Update(dt))
				OnUnitKill(bot->data->father, bot->data);
			bot = bot->next;
		}
	}

	if (starcraftBotList.count() > 0)
	{
		C_List_item<StarcraftBot*>* starcraftBot = NULL;
		starcraftBot = starcraftBotList.start;

		while (starcraftBot)
		{
			if (!starcraftBot->data->Update(dt))
				deadStarcraftBotList.add(starcraftBot->data);
			starcraftBot = starcraftBot->next;
		}
	}

	

	return true;
}
bool M_AI::PostUpdate(float dt)
{
	C_List_item<Bot*>* item;
	C_List_item<Bot*>* item2;

	if (deadStarcraftBotList.count() > 0)
	{
		C_List_item<StarcraftBot*>* _item;
		C_List_item<StarcraftBot*>* _item2;
		for (_item = deadStarcraftBotList.start; _item; _item = _item2)
		{
			_item2 = _item->next;

			if (_item->data->units.count() > 0)
			{
				for (item = _item->data->units.start; item; item = item2)
				{
					item2 = item->next;
					botList.add(item->data);
				}
			}

			deadStarcraftBotList.del(deadStarcraftBotList.At(deadStarcraftBotList.find(_item->data)));
			deadStarcraftBotList.del(_item);
		}
	}

	if (deadBotList.count() > 0)
	{
		for (item = deadBotList.start; item; item = item2)
		{
			item2 = item->next;

			if (item->data->father != NULL)
			{
				item->data->father->units.del(item->data->father->units.At(item->data->father->units.find(item->data)));
			}
			botList.del(botList.At(botList.find(item->data)));
			deadBotList.del(item);
		}
	}

	return true;
}

bool M_AI::CleanUp()
{
	bool ret = true;

	data.clear();
	deadBotList.clear();
	deadStarcraftBotList.clear();
	botList.clear();
	starcraftBotList.clear();

	return ret;
}


Bot* M_AI::CreatePlayerBot(int x, int y, Unit_Type type)
{
	Bot* ret = NULL;
	iPoint tile = App->map->WorldToMap(x, y);

	if (App->pathFinding->IsWalkable(tile.x, tile.y))
	{
		ret = new Bot(x, y, type, playerTeam, NULL);
		
		std::pair<const char*, std::map<const char*, SimpleCVar >> entity_stats;
		App->AI->GetEntityData(type, &entity_stats);
		ret->SetStats(entity_stats, 4);

		AddBot(ret);
	}

	return ret;
}

Bot* M_AI::CreateEnemyBot(int x, int y, Unit_Type type, StarcraftBot* father)
{
	Bot* ret = NULL;
	iPoint tile = App->map->WorldToMap(x, y);

	if (App->pathFinding->IsWalkable(tile.x, tile.y))
	{
		ret = new Bot(x, y, type, playerTeam + 1.0f, NULL);

		std::pair<const char*, std::map<const char*, SimpleCVar >> entity_stats;
		App->AI->GetEntityData(type, &entity_stats);
		ret->SetStats(entity_stats, 4);

		AddBot(ret);
	}

	return ret;
}

StarcraftBot* M_AI::CreateStarcraftBot(int x, int y)
{
	StarcraftBot* ret = NULL;

	//TODO 4:Create a StarcraftBot

	ret = new StarcraftBot(x, y, playerTeam + 1.0f);
	AddStarcraftBot(ret);

	return ret;
}

bool M_AI::GetEntityData(Unit_Type type, std::pair<const char*, std::map < const char*, SimpleCVar >>* stats)
{
	bool ret = true;
	int count = int(type);

	std::map<const char*, std::map<const char*, SimpleCVar>>::iterator it = data.begin()->second.begin();
	for (int i = 0; i < count && it != data.begin()->second.end(); i++)
		++it;

	stats->first = it->first;
	stats->second = it->second;

	return ret;
}

void M_AI::OnUnitKill(StarcraftBot* father, Bot* unit)
{
	if (father)
	{
		father->OnUnitKill(unit);
	}

	deadBotList.add(unit);
}

void M_AI::OnUnitDanger(StarcraftBot* father, Bot* unit)
{
	if (father && unit && father->endangeredUnits.find(unit) == -1)
	{
		father->endangeredUnits.add(unit);
	}
}

void M_AI::OnUnitIdle(StarcraftBot* father, Bot* unit)
{
	if (father && unit)
	{
		father->idleUnits.add(unit);
	}
}

bool M_AI::GetEnemies(float team, C_List<Bot*>* enemies)
{
	C_List_item<Bot*>* item = botList.start;
	while (item)
	{
		if (item->data->unit->team != team)
			enemies->add(item->data);
		item = item->next;
	}

	return (enemies->count() > 0);
}

bool M_AI::AddBot(Bot* bot)
{
	bool ret = true;

	if (ret = (bot != NULL))
		botList.add(bot);

	return ret;
}

bool M_AI::AddStarcraftBot(StarcraftBot* starcraftBot)
{
	bool ret = true;

	if (ret = (starcraftBot != NULL))
		starcraftBotList.add(starcraftBot);

	return ret;
}