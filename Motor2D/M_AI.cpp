
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
	if (deadBotList.count() > 0)
		deadBotList.clear();

	if (deadStarcraftBotList.count() > 0)
		deadStarcraftBotList.clear();

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


Bot* M_AI::CreateBot(int x, int y, Unit_Type type, float team)
{
	Bot* ret = NULL;
	iPoint tile = App->map->WorldToMap(x, y);

	if (App->pathFinding->IsWalkable(tile.x, tile.y))
	{
		ret = new Bot(x, y, type, team, NULL);

		std::pair<const char*, std::map<const char*, SimpleCVar >> entity_stats;
		App->AI->GetEntityData(type, &entity_stats);
		ret->SetStats(entity_stats, 4);
		AddBot(ret);
	}

	return ret;
}

StarcraftBot* M_AI::CreateStarcraftBot(int x, int y, float team)
{
	StarcraftBot* ret = NULL;

	ret = new StarcraftBot(x, y, team);
	
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
	if (!father)
	{
		deadBotList.add(unit);
	}
	else
	{
		father->OnUnitKill(unit);
	}
}

void M_AI::OnUnitDanger(StarcraftBot* father, Bot* unit)
{
	if (father && unit)
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

C_List<Bot*> M_AI::GetEnemies(float team)
{
	C_List<Bot*> ret;
	C_List_item<Bot*>* item = NULL;
	item = botList.start;
	while (item)
	{
		if (item->data->unit->team != team)
			ret.add(item->data);
		item = item->next;
	}
	return ret;
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