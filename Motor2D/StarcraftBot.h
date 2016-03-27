#ifndef __STARCRAFTBOT_H__
#define __STARCRAFTBOT_H__

#include "M_EntityManager.h"

class StarcraftBot
{
	StarcraftBot(){}
	StarcraftBot(float x, float y, uint team){}
	~StarcraftBot(){}

	bool Update(){
		return true;
	}
};

#endif //__STARCRAFTBOT_H__

/*
#include "Bot.h"
#include "StarcraftBot.h"
*/