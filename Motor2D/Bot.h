#ifndef __BOT_H__
#define __BOT_H__


#include "M_EntityManager.h"

class Bot
{
	Bot(){}
	Bot(float x, float y, uint team){}
	~Bot(){}

	bool Update(){
		return true;
	}
};

#endif //__BOT_H__