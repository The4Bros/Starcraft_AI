#ifndef __BOT_H__
#define __BOT_H__

#include "M_EntityManager.h"
#include "M_AI.h"
#include "Unit.h"
#include "j1Timer.h"
#include "StarcraftBot.h"

enum BotState
{
	idle = 0,
	attack,
	flee
};

class StarcraftBot;

class Bot
{

public:

	Bot();
	Bot(int x, int y, Unit_Type type, float team, StarcraftBot* father);
	~Bot();

	bool Update(float dt);
	bool FixedUpdate();

	bool KiteFromEnemy(int attackrange);
	bool CheckForEnemies();
	void FindPlaceToFight();
	float DistanceBetweenUnits(Unit* unit1, Unit* unit2);
	C_Vec2<float> DistanceWithTarget();
	void FollowTarget();
	bool EnemyOnRange(Unit* unit1, Unit* unit2, float range);

	//Getters & Setters
	fPoint	GetPos();
	float	GetTeam();
	bool	SetStats(std::pair<const char*, std::map<const char*, SimpleCVar >> entity_stats, int index);
	void	SetState(BotState _state);

	//Triggers
	void OnAttack(int damage, Bot* attacker);
	void OnKill();
	
	//Orders
	void Attack(Bot* target);
	void GoTo(fPoint pos);

public:

	Unit* unit;
	Bot* target;
	fPoint targetPos;

	StarcraftBot* father;

	fPoint initialPos;
	j1Timer updateTimer;
	j1Timer attackTimer;

	float fixedUpdateSeconds = 0.0f;
	float attackSpeed = 0.0f;

	BotState state = idle;
	int damage = 0;
	float sightRange = 0.0f;
	float attackRange = 0.0f;
	
	bool kites = false;
	bool flees = false;
	
};

#endif //__BOT_H__