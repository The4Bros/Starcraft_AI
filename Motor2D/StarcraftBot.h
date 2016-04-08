#ifndef __STARCRAFTBOT_H__
#define __STARCRAFTBOT_H__

#include "M_EntityManager.h"
#include "j1Timer.h"
#include "C_List.h"
#include "M_AI.h"

class Bot;

class StarcraftBot
{

public:

	StarcraftBot();
	StarcraftBot(int x, int y, float team);
	~StarcraftBot();

	bool Update(float dt);
	bool FixedUpdate();

	void OnUnitKill(Bot* unit);
	void OnUnitDanger(Bot* unit);
	void OnUnitIdle(Bot* unit);

public:

	float team;
	fPoint base;
	int maxPopulation = 4;


	j1Timer updateTimer;
	float fixedUpdateSeconds = 0.05f;

	j1Timer spawnTimer;
	float SpawnTime = 0.5f;

	bool attacking = false;


	C_List<Bot*> units;
	C_List<Bot*> endangeredUnits;
	C_List<Bot*> idleUnits;
};

#endif //__STARCRAFTBOT_H__
