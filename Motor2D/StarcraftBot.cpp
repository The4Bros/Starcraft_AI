#include "StarcraftBot.h"
#include "Bot.h"
#include "j1App.h"
#include "M_EntityManager.h"
#include "M_AI.h"
#include "Unit.h"


StarcraftBot::StarcraftBot()
{

}
StarcraftBot::StarcraftBot(int x, int y, float team)
{
}

StarcraftBot::~StarcraftBot()
{

}

bool StarcraftBot::Update(float dt)
{
	bool ret = true;

	if (fixedUpdateSeconds <= updateTimer.ReadSec())
	{
		FixedUpdate();
		updateTimer.Start();
	}

	return ret;
}

bool StarcraftBot::FixedUpdate()
{
	bool ret = true;

	return ret;
}