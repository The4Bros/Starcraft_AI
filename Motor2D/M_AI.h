#ifndef __M_AI_H__
#define __M_AI_H__

#include "j1Module.h"

class Unit;
enum Unit_Type;

class M_AI : public j1Module
{
public:
	M_AI(bool);
	~M_AI();
	/*
	bool PreStart(pugi::xml_node& node);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void OnGUI(GUI_EVENTS, UI_Element*);
	void ManageInput();
	*/
public:


private:

};

#endif //__M_AI_H__