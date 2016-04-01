#ifndef __CONTROLLED_H__
#define __CONTROLLED_H__

#include "C_Point.h"
#include "C_Vec2.h"
#include "C_DynArray.h"

#include "Entity.h"

class UI_ProgressBar;
class UI_Image;

class Controlled : public Entity
{
public:
	Controlled();
	~Controlled();

	virtual bool Start();
	virtual bool Update(float dt);

	//Drawing methods
	virtual void Draw();
	virtual void DrawDebug();

	void SetHP(int newHP);
	void SetFlying(bool fly);

	int GetHP() const;
	int GetMaxHP() const;
	void Hit(int damage);

	//HP controllers
	virtual void CreateBar();
	void UpdateBarState();
	void UpdateBarPosition();
	void UpdateBarTexture();

protected:
	int maxHP = 100;
	int currHP = 100;

	bool flying = false;
	UI_Image* HPBar_Empty;
	UI_ProgressBar* HPBar_Filled;

};

#endif //__CONTROLLED_H__