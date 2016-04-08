#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "j1Module.h"
#include <map>
#include "SimpleCVar.h"

class Unit;
class Bot;
enum Unit_Type;

class M_EntityManager : public j1Module
{
public:
	M_EntityManager(bool);
	~M_EntityManager();

	bool PreStart(pugi::xml_node& node);
	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void DoUnitLoop(float dt);
	void UpdateSelectionRect();

	void ManageInput();

	Unit* CreateUnit(int x, int y, Unit_Type type, float team, Bot* father);

	bool deleteUnit(Unit* item);
	bool deleteUnit(C_List_item<Unit*>* item);
	bool IsUnitSelected(C_List_item<Unit*>*);
	bool Order(int x, int y);
	void SendNewPath(int x, int y);
	void SendNewPath(int x, int y, C_List<Unit*> units);
	void SendNewPath(int x, int y, Unit* unit);

	SDL_Texture* GetTexture(Unit_Type type);

	void DrawDebug();
	
public:
	bool continuous = true;
	bool smooth = false;
	bool groupMovement = true;

	bool selectUnits = false;

	SDL_Rect selectionRect;
	SDL_Rect groupRect;
	SDL_Rect destinationRect;

	C_List<Unit*> unitList;
	C_List<Unit*> selectedUnits;
	C_List<Unit*> unitsToDelete;

	SDL_Texture* entity_tex;
	SDL_Texture* bot_tex;
	SDL_Texture* unit_base;
	SDL_Texture* unit_red_base;
	SDL_Texture* path_tex;
	SDL_Texture* hpBar_empty;
	SDL_Texture* hpBar_filled;
	SDL_Texture* hpBar_mid;
	SDL_Texture* hpBar_low;

	//Collision variables
	int currentPriority = 1;

private:

	void AddUnit(Unit* unit);

};

#endif //_ENTITYMANAGER_H__