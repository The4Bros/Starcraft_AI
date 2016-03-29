#ifndef __M_AI_H__
#define __M_AI_H__

#include "j1Module.h"
#include <map>
#include "M_EntityManager.h"
#include "SimpleCVar.h"
#include "Bot.h"
#include "StarcraftBot.h"

class Bot;
class StarcraftBot;

class M_AI : public j1Module
{
public:
	M_AI(bool);
	~M_AI();
	
	bool PreStart(pugi::xml_node& node);
	//bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	//void OnGUI(GUI_EVENTS, UI_Element*);
	//void ManageInput();

	Bot* CreateBot(int x, int y, Unit_Type type, float team);
	StarcraftBot* CreateStarcraftBot(int x, int y);

	bool GetEntityData(Unit_Type type, std::pair<const char*, std::map < const char*, SimpleCVar >>* stats);

public:

	C_List<Bot*> botList;
	C_List<StarcraftBot*> starcraftBotList;

	C_List<StarcraftBot*> deadStarcraftBotList;
	C_List<Bot*> deadBotList;

private:

	bool AddBot(Bot* bot);
	bool AddStarcraftBot(StarcraftBot* starcraftBot);

private:

	std::map<const char*, std::map<const char*, std::map<const char*, SimpleCVar>>> data;

};

#endif //__M_AI_H__