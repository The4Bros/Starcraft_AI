#ifndef __M_AI_H__
#define __M_AI_H__

#include "j1Module.h"
#include "Bot.h"
#include "StarcraftBot.h"
#include <map>
#include <string.h>

class Unit;

enum SCVarTypes
{
	sc_float = 0,
	sc_int,
	sc_string,
	sc_bool,
};

class SimpleCVar
{
public:
	SimpleCVar(float _value);
	SimpleCVar(int _value);
	SimpleCVar(char* _value);
	SimpleCVar(bool _value);
	SimpleCVar(){};

private:
	union {
		float floatVar;
		int intVar;
		char* stringVar;
		bool boolVar;
	} value;

	SCVarTypes type;

public:
	void Set(float newValue);
	void Set(int newValue);
	void Set(char* newValue);
	void Set(bool newValue);

	SCVarTypes GetType();

	void Read(void* ret, SCVarTypes expectedData);
	bool Read(float* output);
	bool Read(int* output);
	bool Read(char* output);
	bool Read(bool* output);
	void* ForceRead();

	void Display();
};

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

	//StarcraftBot* AddStarcraftBot(...);
	//Bot*			AddBot(...);

	bool Read(const char* arg1, const char* arg2, const char* arg3, float* value);
	bool Read(const char* arg1, const char* arg2, const char* arg3, int* value);
	bool Read(const char* arg1, const char* arg2, const char* arg3, char* value);
	bool Read(const char* arg1, const char* arg2, const char* arg3, bool* value);
	bool Read(const char* arg1, const char* arg2, char* name);

private:

	std::map<const char*, std::map<const char*, std::map<const char*, SimpleCVar>>> data;

};

#endif //__M_AI_H__