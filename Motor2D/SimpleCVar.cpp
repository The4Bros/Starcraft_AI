#include "SimpleCVar.h"
#include "Defs.h"
#include "Log.h"

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
	value.boolVar = _value;
}


void SimpleCVar::Set(float newValue)
{
	if (type == sc_float)
	{
		value.floatVar = newValue;
	}
}
void SimpleCVar::Set(int newValue)
{
	if (type == sc_int)
	{
		value.intVar = newValue;
	}
}
void SimpleCVar::Set(char* newValue)
{
	if (type == sc_string)
	{
		value.stringVar = newValue;
	}
}
void SimpleCVar::Set(bool newValue)
{
	if (type == sc_bool)
	{
		value.boolVar = newValue;
	}
}

SCVarTypes SimpleCVar::GetType()
{
	return type;
}

bool	SimpleCVar::Read(float* output)		{ if (type == sc_float)	{ *output = value.floatVar; return true; }	return false; }
bool	SimpleCVar::Read(int* output)		{ if (type == sc_int)	{ *output = value.intVar;	return true; }	return false; }
bool	SimpleCVar::Read(char* output)		{ if (type == sc_string)	{ output = value.stringVar; return true; }	return false; }
bool	SimpleCVar::Read(bool* output)		{ if (type == sc_bool)	{ *output = value.boolVar;	return true; }	return false; }
void*	SimpleCVar::ForceRead()				{ return &value; }

void SimpleCVar::Display()
{
	switch (type)
	{
	case sc_float:
		LOG("Float value: %f", value.floatVar);
		break;
	case sc_int:
		LOG("Int value: %i", value.intVar);
		break;
	case sc_string:
		LOG("String value: %s", value.stringVar);
		break;
	case sc_bool:
		LOG("Bool value: %b", value.boolVar);
		break;
	default:
		break;
	}
}