// ----------------------------------------------------
// SimpleCVar without reference -----------------------
// ----------------------------------------------------

#ifndef __SIMPLECVAR_H__
#define __SIMPLECVAR_H__

#include <map>

enum SCVarTypes
{
	sc_float = 0,
	sc_int,
	sc_string,
	sc_bool,
	undefined
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

	SCVarTypes type = undefined;

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

#endif //__SIMPLECVAR_H__