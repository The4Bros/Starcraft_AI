#include "M_CollisionController.h"

#include "j1App.h"

#include "M_Input.h"
#include "M_Textures.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include "Unit.h"
#include "Entity.h"
#include "M_EntityManager.h"
#include "UIElements.h"

M_CollisionController::M_CollisionController(bool start_enabled) : j1Module(start_enabled)
{
	name.create("collision_controller");
}

// Destructor
M_CollisionController::~M_CollisionController()
{}

// Called before render is available
bool M_CollisionController::Awake(pugi::xml_node& node)
{

	return true;
}

// Called before the first frame
bool M_CollisionController::Start()
{
	return true;
}

// Called each loop iteration
bool M_CollisionController::PreUpdate()
{

	return true;
}

// Called each loop iteration
bool M_CollisionController::Update(float dt)
{
	DoUnitLoop();
	return true;
}

// Called each loop iteration
bool M_CollisionController::PostUpdate(float dt)
{
	bool ret = true;
	
	return ret;
}

bool M_CollisionController::CleanUp()
{
	return true;
}

void M_CollisionController::ManageInput(float dt)
{

}

void M_CollisionController::DoUnitLoop()
{
	for (int i = 0; i < App->entityManager->unitList.count(); i++)
	{
		Unit* unit = App->entityManager->unitList[i];
		//If unit is moving
		if (!App->entityManager->unitList[i]->targetReached)
		{
			if (mapChanged)
			{
				bool stop = false;
				for (int n = App->entityManager->unitList[i]->currentNode; n < App->entityManager->unitList[i]->path.Count(); n++)
				{
					if (!App->pathFinding->IsWalkable(unit->path[n].x, unit->path[n].y))
					{
						stop = true;
						C_DynArray<iPoint> newPath;
						iPoint unitPos = App->map->WorldToMap(unit->GetPosition().x, unit->GetPosition().y);
						App->pathFinding->GetNewPath(unitPos, unit->path[unit->path.Count() - 1], newPath);
						unit->SetNewPath(newPath);
					}
				}
				
			}
		}

		//If unit is not moving
		else
		{
			iPoint unitPos = App->map->WorldToMap(unit->GetPosition().x, unit->GetPosition().y);
			if (!App->pathFinding->IsWalkable(unitPos.x, unitPos.y))
			{
				LOG("Unit in no-walkable tile");
				//: If the unit is in a non-walkable tile find a nearby walkable tile and repath
				iPoint tile = FindClosestWalkable(unitPos.x, unitPos.y);
				iPoint dst = App->map->MapToWorld(tile.x, tile.y);
				unit->SetTarget(dst.x, dst.y);
				unit->path.Clear();
			}
			else
			{
				bool stop = false;
				for (int n = 0; n < App->entityManager->unitList.count() && !stop; n++)
				{
					if (i != n  && App->entityManager->unitList[n]->targetReached)
					{
						//: If two units are colliding, separe them
						Unit* unit2 = App->entityManager->unitList[n];
						if (DoUnitsIntersect(unit, unit2))
						{
							if (unit->priority > unit2->priority)
								SplitUnits(unit, unit2);
							else
							{
								SplitUnits(unit2, unit);
								stop = true;
							}
						}
					}
				}
			}
		}
	}
	if (mapChanged)
		mapChanged = false;
}


iPoint M_CollisionController::FindClosestWalkable(int x, int y)
{
	bool found = false;
	int lenght = 3;

	iPoint tile = { x + lenght / 2, y - lenght / 2 - 1 };

	while (!found && lenght < 20)
	{
		while (tile.y < y + lenght / 2 && !found)
		{
			tile.y++;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;
		}

		while (tile.x > x - lenght / 2 && !found)
		{
			tile.x--;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;
		}

		while (tile.y > y - lenght / 2 && !found)
		{
			tile.y--;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;

		}

		while (tile.x < x + lenght / 2 && !found)
		{
			tile.x++;
			if (App->pathFinding->IsWalkable(tile.x, tile.y))
				found = true;
		}
		lenght += 2;
	}
	return tile;
}

bool M_CollisionController::DoUnitsIntersect(Unit* unit1, Unit* unit2)
{
	C_Vec2<float> distance = { unit1->GetPosition().x - unit2->GetPosition().x, unit1->GetPosition().y - unit2->GetPosition().y };
	return (distance.GetModule() < unit1->colRadius + unit2->colRadius);
}

/*
Higher priority unit is unit1, we will move unit2
Searches a walkable tile for unit2 and sends it there
*/
void M_CollisionController::SplitUnits(Unit* unit1, Unit* unit2)
{
	C_Vec2<float> vec = { unit2->GetPosition().x - unit1->GetPosition().x, unit2->GetPosition().y - unit1->GetPosition().y };
	vec.position = unit1->GetPosition();
	vec.Normalize();
	vec *= unit1->colRadius + unit2->colRadius + 1;

	if (vec.GetModule() == 0)
	{
		vec.x = unit1->colRadius + unit2->colRadius + 1;
		vec.SetAngle(rand() % 360);
	}

	fPoint pos = vec.position + fPoint{vec.x, vec.y};
	iPoint tile = App->map->WorldToMap(pos.x, pos.y);
	iPoint dst = App->map->MapToWorld(tile.x, tile.y);

	int loops = 0;
	while (!App->pathFinding->IsWalkable(tile.x, tile.y) && loops < 24)
	{
		vec.SetAngle(vec.GetAngle() + 15);
		pos = vec.position + fPoint{ vec.x, vec.y };
		tile = App->map->WorldToMap(pos.x, pos.y);
		dst = App->map->MapToWorld(tile.x, tile.y);
		loops++;
	}

	unit2->SetTarget(round(pos.x), round(pos.y));
	unit2->path.Clear();
}