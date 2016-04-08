#ifndef __UNIT_H__
#define __UNIT_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"
#include "SimpleCVar.h"
#include <map>
#include "j1Timer.h"

#include "Entity.h"
#include "Controlled.h"

class UIBar;
class Bot;

enum Unit_Type
{
	unit_1 = 0,
	unit_2,
	unit_3
};

enum DamageType
{
	normal,
	splash,
};


class Unit : public Controlled
{
public:
	Unit();
	Unit(float x, float y, float team, Bot* father);
	Unit(fPoint);
	~Unit();

	bool Start();
	bool Update(float dt);

	//Movement methods
	bool UpdateVelocity(float dt);
	void GetDesiredVelocity();

	bool Rotate(float dt);
	bool Move(float dt);
	void Stop();

	bool GetNewTarget();
	bool isTargetReached();
	bool isAngleReached();

	//Setters
	void SetTarget(int x, int y);
	void SetNewPath(C_DynArray<iPoint>& newPath);
	void SetType(Unit_Type _type);
	void SetMaxSpeed(float speed);
	void SetPriority(int priority);
	bool SetStats(std::pair<const char*, std::map<const char*, SimpleCVar >> entity_stats);
	bool SetDirection(C_Vec2<float> dir);

	//Getters
	void GetTextureRect(SDL_Rect&, SDL_RendererFlip&) const;
	Unit_Type GetType();
	
	void UpdateCollider();
	void Destroy();

	//Drawing methods
	void Draw();
	void DrawDebug();

	void Target();

	void Hit(int damage, DamageType type);

private:

	void SetPosToTarget();

public:

	//Collision variables
	int priority;
	int colRadius = 5 * 4;

	C_DynArray<iPoint> path;
	iPoint target;
	int currentNode = 0;
	bool targetReached = true;
	float team;
	Bot* father;

private:

	Unit_Type type;

	//Velocities
	C_Vec2<float> currentVelocity = { 0, 0 };
	C_Vec2<float> desiredVelocity = { 0, 0 };

	//Movement variables
	float maxSpeed =  150.0f; //Big max speed could get bugged
	float rotationSpeed = 360.0f; //Used as angles / seconds
	float targetRadius = 2.0f;

	bool targeted = false;
	j1Timer targetedTimer;
	int targetedTimerInterval = 1;
	int targetedTimerDuration = 15;
};

#endif //__UNIT_H__