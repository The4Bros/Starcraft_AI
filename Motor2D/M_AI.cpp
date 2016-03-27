
#include "M_AI.h"

#include "Defs.h"
#include "Log.h"

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
				case 0: ly1_pair.second = SimpleCVar(stat.attribute("value").as_float());		break;
				case 1: ly1_pair.second = SimpleCVar(stat.attribute("value").as_int());			break;
				case 2: ly1_pair.second = SimpleCVar(stat.attribute("value").as_string(""));	break;
				case 3: ly1_pair.second = SimpleCVar(stat.attribute("value").as_bool());		break;
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

bool M_AI::CleanUp()
{
	bool ret = true;

	data.clear();

	return ret;
}

bool M_AI::Read(const char* arg1, const char* arg2, const char* arg3, float* value)
{
	bool ret = true;
	SimpleCVar* var = NULL;

	try
	{
		var = &data.at(arg1).at(arg2).at(arg3);
	}
	catch (const std::out_of_range& oor)
	{
		ret = false;
	}

	if (ret)
		var->Read(value);

	return ret;
}

bool M_AI::Read(const char* arg1, const char* arg2, const char* arg3, int* value)
{
	bool ret = true;
	SimpleCVar* var = NULL;

	try
	{
		var = &data.at(arg1).at(arg2).at(arg3);
	}
	catch (const std::out_of_range& oor)
	{
		ret = false;
	}

	if (ret)
		var->Read(value);

	return ret;
}

bool M_AI::Read(const char* arg1, const char* arg2, const char* arg3, char* value)
{
	bool ret = true;
	SimpleCVar* var = NULL;

	try
	{
		var = &data.at(arg1).at(arg2).at(arg3);
	}
	catch (const std::out_of_range& oor)
	{
		ret = false;
	}

	if (ret)
		var->Read(value);

	return ret;
}

bool M_AI::Read(const char* arg1, const char* arg2, const char* arg3, bool* value)
{
	bool ret = true;
	SimpleCVar* var = NULL;

	try
	{
		var = &data.at(arg1).at(arg2).at(arg3);
	}
	catch (const std::out_of_range& oor)
	{
		ret = false;
	}

	if (ret)
		var->Read(value);

	return ret;
}


SimpleCVar::SimpleCVar(float _value) : type(sc_float)
{
	value.floatVar = _value;
}

SimpleCVar::SimpleCVar(int _value) : type(sc_int)
{
	value.intVar = _value;
}

SimpleCVar::SimpleCVar(char* _value) : type(sc_string)
{
	value.stringVar = _value;
}

SimpleCVar::SimpleCVar(bool _value) : type(sc_bool)
{
	value.floatVar = _value;
}


void SimpleCVar::Set(float newValue)
{
	if (type == c_float)
	{
		value.floatVar = newValue;
	}
}
void SimpleCVar::Set(int newValue)
{
	if (type == c_int)
	{
		value.intVar = newValue;
	}
}
void SimpleCVar::Set(char* newValue)
{
	if (type == c_string)
	{
		value.stringVar = newValue;
	}
}
void SimpleCVar::Set(bool newValue)
{
	if (type == c_bool)
	{
		value.boolVar = newValue;
	}
}

SCVarTypes SimpleCVar::GetType()
{
	return type;
}

bool	SimpleCVar::Read(float* output)		{ if (type == c_float)	{ *output = value.floatVar; return true; }	return false; }
bool	SimpleCVar::Read(int* output)		{ if (type == c_int)	{ *output = value.intVar;	return true; }	return false; }
bool	SimpleCVar::Read(char* output)		{ if (type == c_string)	{ output = value.stringVar; return true; }	return false; }
bool	SimpleCVar::Read(bool* output)		{ if (type == c_bool)	{ *output = value.boolVar;	return true; }	return false; }
void*	SimpleCVar::ForceRead()				{ return &value; }

void SimpleCVar::Display()
{
	switch (type)
	{
	case c_float:
		LOG("Float value: %f", value.floatVar);
		break;
	case c_int:
		LOG("Int value: %i", value.intVar);
		break;
	case c_string:
		LOG("String value: %s", value.stringVar);
		break;
	case c_bool:
		LOG("Bool value: %b", value.boolVar);
		break;
	default:
		break;
	}
}