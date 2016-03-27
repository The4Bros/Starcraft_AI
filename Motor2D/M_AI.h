#ifndef __M_AI_H__
#define __M_AI_H__

#include "j1Module.h"
#include "Bot.h"
#include "StarcraftBot.h"
#include <map>
#include "SimpleCVar.h"

class Bot;
class StarcraftBot;

class M_AI : public j1Module
{
public:
	M_AI(bool);
	~M_AI();
	
	bool PreStart(pugi::xml_node& node);
	//bool Start();
	//bool Update(float dt);
	//bool PostUpdate(float dt);
	bool CleanUp();

	//void OnGUI(GUI_EVENTS, UI_Element*);
	//void ManageInput();

	Bot*			AddBot(const char* name, int x, int y);
	StarcraftBot*	AddStarcraftBot(int x, int y);

private:

	bool Read(const char* arg1, const char* arg2, const char* arg3, float* value);
	bool Read(const char* arg1, const char* arg2, const char* arg3, int* value);
	bool Read(const char* arg1, const char* arg2, const char* arg3, char* value);
	bool Read(const char* arg1, const char* arg2, const char* arg3, bool* value);
	bool Read(const char* arg1, const char* arg2, char* name);

private:

	std::map<const char*, std::map<const char*, std::map<const char*, SimpleCVar>>> data;

};

#endif //__M_AI_H__