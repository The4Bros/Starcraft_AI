#ifndef __STARCRAFTBOT_H__
#define __STARCRAFTBOT_H__

#include "M_EntityManager.h"
#include "j1Timer.h"
#include "C_List.h"

class StarcraftBot
{

public:

	StarcraftBot();
	StarcraftBot(int x, int y, float team);
	~StarcraftBot();

	bool Update(float dt);
	bool FixedUpdate();

public:

	fPoint base;

	j1Timer updateTimer;
	float fixedUpdateSeconds = 0.25f;

	C_List<Unit*> units;
	//C_List<Building*> buildings;
};

#endif //__STARCRAFTBOT_H__

/*
#include "Bot.h"
#include "StarcraftBot.h"
*/