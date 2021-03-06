#include "M_EntityManager.h"

#include "j1App.h"
#include "M_Textures.h"
#include "Unit.h"
#include "M_Render.h"
#include "M_Input.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include "S_SceneAI.h"
#include "M_CollisionController.h"
#include "SimpleCVar.h"
#include "M_AI.h"

M_EntityManager::M_EntityManager(bool start_enabled) : j1Module(start_enabled)
{
	name.create("entity_manager");
}

M_EntityManager::~M_EntityManager()
{

}
bool M_EntityManager::PreStart(pugi::xml_node& node)
{
	entity_tex = App->tex->Load("graphics/protoss/units/plane.png");
	bot_tex = App->tex->Load("graphics/protoss/units/plane_BOT.png");
	unit_base = App->tex->Load("graphics/ui/o062.png");
	unit_red_base = App->tex->Load("graphics/ui/o062red.png");
	path_tex = App->tex->Load("textures/path.png");
	hpBar_empty = App->tex->Load("graphics/ui/hpbarempt.png");
	hpBar_filled = App->tex->Load("graphics/ui/hpbarfull.png");
	hpBar_mid = App->tex->Load("graphics/ui/hpbarmid.png");;
	hpBar_low = App->tex->Load("graphics/ui/hpbarlow.png");;
	return true;
}

bool M_EntityManager::Start()
{
	return true;
}

bool M_EntityManager::Update(float dt)
{
	ManageInput();

	DoUnitLoop(dt);
	UpdateSelectionRect();

	if (App->sceneAI->renderForces)
		DrawDebug();

	return true;
}

bool M_EntityManager::PostUpdate(float dt)
{
	if (selectionRect.w != 0 || selectionRect.h != 0)
		App->render->DrawQuad(selectionRect, false, 0, 255, 0, 255, false);

	if (unitsToDelete.count() > 0)
	{
		C_List_item<Unit*>* item;
		C_List_item<Unit*>* item2;
		for (item = unitsToDelete.start; item; item = item2)
		{
			item2 = item->next;
			deleteUnit(item);
		}
	}
	return true;
}

bool M_EntityManager::CleanUp()
{
	C_List_item<Unit*>* item = NULL;
	item = unitList.start;
	while (item)
	{
		delete item->data;
		item = item->next;
	}
	return true;
}

void M_EntityManager::DoUnitLoop(float dt)
{
	C_List_item<Unit*>* item = NULL;
	item = unitList.start;

	while (item)
	{
		item->data->UpdateBarState();
		if (selectUnits)
		{
			if (selectionRect.w != 0 || selectionRect.h != 0)
			{
				//Selecting units
				if (IsUnitSelected(item))
				{
					if (item->data->selected == false)
					{
						item->data->selected = true;
						item->data->UpdateBarState();
						selectedUnits.add(item->data);
					}
				}
				else if (item->data->selected == true)
				{
					item->data->selected = false;
					item->data->UpdateBarState();
					selectedUnits.del(selectedUnits.At(selectedUnits.find(item->data)));
				}
			}
		}

		//Unit update
		if (!item->data->Update(dt))
			unitsToDelete.add(item->data);

		item = item->next;
	}

	if (selectUnits)
	{
		selectionRect.w = selectionRect.h = 0;
		selectUnits = false;
		
		if (selectedUnits.count() != 0)
		{
			bool sort = false;
			uint min = selectedUnits.start->data->team;
			item = selectedUnits.start;

			while (item)
			{
				if (item->data->team != min)
				{
					sort = true;
					min = (item->data->team < min ? item->data->team : min);
				}

				item = item->next;
			}

			if (sort)
			{
				item = selectedUnits.start;

				while (item)
				{
					if (item->next == NULL)
					{
						if (item->data->team != min)
						{
							item->data->selected = false;
							item->data->UpdateBarState();
							selectedUnits.del(item);
						}

						return;
					}
					if (item->data->team != min)
					{
						item->data->selected = false;
						item->data->UpdateBarState();

						item = item->next;
						selectedUnits.del(item->prev);
					}
					else
						item = item->next;
				}
			}
		}
	}
}

void M_EntityManager::UpdateSelectionRect()
{
	if (groupMovement)
	{
		C_List_item<Unit*>* item = NULL;
		item = selectedUnits.start;

		int minX = 100000, minY = 100000, maxX = 0, maxY = 0;
		while (item)
		{
			if (item->data->GetPosition().x < minX)
				minX = item->data->GetPosition().x;
			if (item->data->GetPosition().y < minY)
				minY = item->data->GetPosition().y;
			if (item->data->GetPosition().x > maxX)
				maxX = item->data->GetPosition().x;
			if (item->data->GetPosition().y > maxY)
				maxY = item->data->GetPosition().y;

			item = item->next;
		}

		groupRect = { minX, minY, maxX - minX, maxY - minY };
	}
}

void M_EntityManager::ManageInput()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		App->input->GetMousePosition(selectionRect.x, selectionRect.y);
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		selectionRect.w = x - selectionRect.x;
		selectionRect.h = y - selectionRect.y;
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		selectUnits = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		smooth = !smooth;
		LOG("Changing smooth state");
	}

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
	{
		continuous = !continuous;
		LOG("Changing continuous state");
	}

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
	{
		groupMovement = !groupMovement;
		LOG("Changing groupMovement state");
	}

}

Unit* M_EntityManager::CreateUnit(int x, int y, Unit_Type type, float team, Bot* father)
{
	Unit* unit = NULL;

	iPoint tile = App->map->WorldToMap(x, y);
	bool isWalkable = App->pathFinding->IsWalkable(tile.x, tile.y);

	std::pair<const char*, std::map<const char*, SimpleCVar >> entity_stats;

	if (isWalkable && App->AI->GetEntityData(type, &entity_stats))
	{
		unit = new Unit(x, y, team, father);
		if (unit->SetStats(entity_stats))
		{
			unit->SetType(type);
			unit->SetCollider({ 0, 0, 5 * 8, 5 * 8 });
			unit->SetPriority(currentPriority++);
			unit->Start();
			AddUnit(unit);
		}
		else
		{
			delete[] unit;
			unit = NULL;
		}
	}

	return unit;
}

bool M_EntityManager::deleteUnit(C_List_item<Unit*>* item)
{
	item->data->Destroy();
	if (item->data->selected)
	{
		selectedUnits.del(selectedUnits.At(selectedUnits.find(item->data)));
	}
	unitList.del(unitList.At(unitList.find(item->data)));
	unitsToDelete.del(item);
	return true;
}

bool M_EntityManager::deleteUnit(Unit* item)
{
	bool ret = true;

	if (ret = (item != NULL))
		ret = unitsToDelete.add(item);

	return ret;
}


bool M_EntityManager::IsUnitSelected(C_List_item<Unit*>* unit)
{
	SDL_Rect itemRect = unit->data->GetCollider();
	itemRect.x += App->render->camera.x;
	itemRect.y += App->render->camera.y;
	SDL_Rect rect = selectionRect;

	//Fixing negative values
	if (rect.h < 0)
	{
		rect.y += rect.h;
		rect.h *= -1;
	}
	if (rect.w < 0)
	{
		rect.x += rect.w;
		rect.w *= -1;
	}
	if (SDL_HasIntersection(&rect, &itemRect))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool M_EntityManager::Order(int x, int y)
{
	if (selectedUnits.count() > 0)
	{
		// can't order if not on our team
		if (selectedUnits.start->data->team == App->AI->playerTeam)
		{
			C_List_item<Unit*>* item = unitList.start;
			SDL_Rect itemRect;

			SDL_Rect rect = { x - 3, y - 3, 6, 6 };

			while (item)
			{
				itemRect = item->data->GetCollider();
				itemRect.x += App->render->camera.x;
				itemRect.y += App->render->camera.y;

				if (SDL_HasIntersection(&rect, &itemRect) && item->data->team != App->AI->playerTeam)
				{
					item->data->Target();

					C_List_item<Unit*>* selected = selectedUnits.start;

					while (selected)
					{
						selected->data->father->Attack(item->data->father);
						selected = selected->next;
					}

					return true;
				}

				item = item->next;
			}
		}
	}

	return false;
}

void M_EntityManager::SendNewPath(int x, int y)
{
	if (App->pathFinding->allowPath)
	{
		//---------------------------------------------------------------------
		if (App->pathFinding->IsWalkable(x, y) && selectedUnits.start->data->team == App->AI->playerTeam)
		{
			for (uint i = 0; i < selectedUnits.count(); i++)
			{
				if (groupMovement)
				{
					C_DynArray<iPoint> newPath;

					//Cloning group rectangle to the destination point
					iPoint Rcenter = App->map->MapToWorld(x, y);
					destinationRect = { Rcenter.x - groupRect.w / 2, Rcenter.y - groupRect.h / 2, groupRect.w, groupRect.h };

					//Distance from rectangle position to unit position
					iPoint posFromRect = { 0, 0 };
					posFromRect.x = selectedUnits[i]->GetPosition().x - groupRect.x;
					posFromRect.y = selectedUnits[i]->GetPosition().y - groupRect.y;

					iPoint dstTile = App->map->WorldToMap(destinationRect.x + posFromRect.x, destinationRect.y + posFromRect.y);

					//Unit tile position
					fPoint unitPos = selectedUnits[i]->GetPosition();
					iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));

					//If destination is not walkable, use the player's clicked tile
					if (!App->pathFinding->IsWalkable(dstTile.x, dstTile.y))
						dstTile = { x, y };

					App->pathFinding->GetNewPath(unitTile, dstTile, newPath);
					selectedUnits[i]->SetNewPath(newPath);
				}
				else
				{
					C_DynArray<iPoint> newPath;

					//---------------------------------------------------------
					fPoint unitPos = selectedUnits[i]->GetPosition();
					iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));
					iPoint dstTile = { x, y };
					App->pathFinding->GetNewPath(unitTile, dstTile, newPath);
					selectedUnits[i]->SetNewPath(newPath);
				}
			}
		}
	}
	else
	{
		for (uint i = 0; i < selectedUnits.count(); i++)
		{
			iPoint dst = App->map->MapToWorld(x, y);
			selectedUnits[i]->SetTarget(dst.x, dst.y);
		}
	}
}

void M_EntityManager::SendNewPath(int x, int y, C_List<Unit*> units)
{
	if (App->pathFinding->allowPath)
	{
		//---------------------------------------------------------------------
		if (App->pathFinding->IsWalkable(x, y))
		{
			for (uint i = 0; i < units.count(); i++)
			{
				if (groupMovement)
				{
					C_DynArray<iPoint> newPath;

					//Cloning group rectangle to the destination point
					iPoint Rcenter = App->map->MapToWorld(x, y);
					destinationRect = { Rcenter.x - groupRect.w / 2, Rcenter.y - groupRect.h / 2, groupRect.w, groupRect.h };

					//Distance from rectangle position to unit position
					iPoint posFromRect = { 0, 0 };
					posFromRect.x = units[i]->GetPosition().x - groupRect.x;
					posFromRect.y = units[i]->GetPosition().y - groupRect.y;

					iPoint dstTile = App->map->WorldToMap(destinationRect.x + posFromRect.x, destinationRect.y + posFromRect.y);

					//Unit tile position
					fPoint unitPos = units[i]->GetPosition();
					iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));

					//If destination is not walkable, use the player's clicked tile
					if (!App->pathFinding->IsWalkable(dstTile.x, dstTile.y))
						dstTile = { x, y };

					App->pathFinding->GetNewPath(unitTile, dstTile, newPath);
					units[i]->SetNewPath(newPath);
				}
				else
				{
					C_DynArray<iPoint> newPath;

					//---------------------------------------------------------
					fPoint unitPos = units[i]->GetPosition();
					iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));
					iPoint dstTile = { x, y };
					App->pathFinding->GetNewPath(unitTile, dstTile, newPath);
					units[i]->SetNewPath(newPath);
				}
			}
		}
	}
	else
	{
		for (uint i = 0; i < units.count(); i++)
		{
			iPoint dst = App->map->MapToWorld(x, y);
			units[i]->SetTarget(dst.x, dst.y);
		}
	}
}

void M_EntityManager::SendNewPath(int x, int y, Unit* unit)
{
	if (App->pathFinding->allowPath)
	{
		if (App->pathFinding->IsWalkable(x, y))
		{
			C_DynArray<iPoint> newPath;

			//---------------------------------------------------------
			fPoint unitPos = unit->GetPosition();
			iPoint unitTile = App->map->WorldToMap(round(unitPos.x), round(unitPos.y));
			iPoint dstTile = { x, y };
			App->pathFinding->GetNewPath(unitTile, dstTile, newPath);
			unit->SetNewPath(newPath);
		}
	}
	else
	{
		iPoint dst = App->map->MapToWorld(x, y);
		unit->SetTarget(dst.x, dst.y);
	}
}

SDL_Texture* M_EntityManager::GetTexture(Unit_Type type)
{
	SDL_Texture* ret = NULL;

	switch (type)
	{
	case unit_1: ret = entity_tex;	break;
	case unit_2: ret = bot_tex;		break;
	case unit_3: ret = entity_tex;	break;
	}

	return ret;
}

void M_EntityManager::AddUnit(Unit* unit)
{
	C_List_item<Unit*>* item = NULL;
	C_List_item<Unit*>* unitItem = new C_List_item<Unit*>(unit);

	bool keepGoing = true;
	for (item = unitList.end; item && keepGoing; item = item->prev)
	{
		if (item->data->GetPosition().y < unit->GetPosition().y)
		{
			unitList.Insert(item, unitItem);
			keepGoing = false;
		}
	}
	if (keepGoing)
		unitList.Insert(NULL, unitItem);
}

void M_EntityManager::DrawDebug()
{
	if (selectedUnits.count() > 0)
		App->render->DrawQuad(groupRect, true, 255, 0, 0, 255, false);

	App->render->DrawQuad(destinationRect, true, 0, 0, 255, 255, false);
}
