
#include "M_EntityManager.h"

class Bot
{
	Bot();
	Bot(float x, float y, uint team);
	~Bot();

	uint state;

	bool Update();
};