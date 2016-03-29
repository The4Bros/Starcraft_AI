#ifndef __BOT_H__
#define __BOT_H__

#include "M_EntityManager.h"
#include "M_AI.h"
#include "Unit.h"
#include "j1Timer.h"

enum BotState
{
	idle = 0,
	attack,
	kite,
	flee
};

class Bot
{

public:

	Bot();
	Bot(int x, int y, Unit_Type type, float team);
	~Bot();

	bool Update(float dt);
	bool FixedUpdate();
	bool SetStats(std::pair<const char*, std::map<const char*, SimpleCVar >> entity_stats, int index);

	fPoint GetPos();
	bool CheckForEnemies(float range);
	bool TargetOnRange(float range);
	void FollowTarget();
	bool EnemyOnUnitRange(Unit* unit1, Unit* unit2, float range);
	void SetState(BotState newstate);

	void OnAttack(int damage, Bot* attacker);
	void OnDeath();

public:

	Unit* unit;
	Unit* target;
	fPoint targetLastPos;

	j1Timer updateTimer;
	j1Timer attackTimer;

	float fixedUpdateSeconds = 0.25f;

	BotState state = idle;
	int damage = 0;
	float sightRange = 0.0f;
	float attackRange = 0.0f;
	bool kites = false;
	bool flees = false;
	
};

#endif //__BOT_H__