#include "S_SceneAI.h"

#include "j1App.h"
#include "M_Input.h"
#include "M_Textures.h"
//#include "M_Audio.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_Map.h"
#include "M_PathFinding.h"
#include "M_GUI.h"
#include "M_EntityManager.h"
#include "Entity.h"
#include "Unit.h"
//#include "j1Gui.h"
//#include "UIElements.h"
//#include "M_Fonts.h"
#include "M_Console.h"
#include "M_AI.h"

S_SceneAI::S_SceneAI(bool start_enabled) : j1Module(start_enabled)
{
	name.create("scene_ai");
}

// Destructor
S_SceneAI::~S_SceneAI()
{}

// Called before render is available
bool S_SceneAI::Awake(pugi::xml_node& node)
{

	LOG("Loading Scene");
	bool ret = true;

	App->SetCurrentScene(this);

	App->console->AddCommand(&c_SaveGame);
	App->console->AddCommand(&c_LoadGame);

	return ret;
}

// Called before the first frame
bool S_SceneAI::Start()
{
	team = 0;

	pugi::xml_node config = App->GetConfig("scene");
	App->GetConfig("scene");

	App->map->Load("sc-jungle.tmx");

	//LoadGUI();

	debug_tex = App->tex->Load("textures/current_tile.png");
	collision_tex = App->tex->Load("textures/collision_tile.png");
	mapTexture = App->tex->Load("maps/empty_map.png");

	return true;
}

// Called each loop iteration
bool S_SceneAI::PreUpdate()
{
	//Getting current tile
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	currentTile_x = p.x;
	currentTile_y = p.y;

	return true;
}

// Called each loop iteration
bool S_SceneAI::Update(float dt)
{
	ManageInput(dt);

	SDL_Rect rect1 = { 0, 0, 0, 0 };
	App->render->Blit(mapTexture, &rect1, true);

	if (renderMap)
		App->map->Draw();
	
	//Render current tile
	iPoint p = App->map->MapToWorld(currentTile_x, currentTile_y);
	SDL_Rect pos = { p.x, p.y, 8, 8 };
	SDL_Rect rect = { 0, 0, 64, 64 };
	App->render->Blit(debug_tex, &pos, true, &rect);

	return true;
}

// Called before quitting
bool S_SceneAI::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void S_SceneAI::ManageInput(float dt)
{
	if (App->input->GetInputState() == false)
	{
		//Enable / Disable forces debug
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
			renderForces = !renderForces;

		if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_DOWN)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			iPoint p = App->render->ScreenToWorld(x, y);
			p = App->map->WorldToMap(p.x, p.y);
			p = App->map->MapToWorld(p.x, p.y);
			App->AI->CreateBot(p.x + 4, p.y + 4, unit_1, team);
		}
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			int x, y;
			App->input->GetMousePosition(x, y);
			iPoint p = App->render->ScreenToWorld(x, y);
			p = App->map->WorldToMap(p.x, p.y);
			p = App->map->MapToWorld(p.x, p.y);
			App->AI->CreateStarcraftBot(p.x + 4, p.y + 4, team + 1);
		}
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			App->entityManager->SendNewPath(currentTile_x, currentTile_y);
		}
	}
}

void S_SceneAI::LoadGUI()
{
	
}

void S_SceneAI::OnGUI(GUI_EVENTS event, UI_Element* element)
{

}

void::S_SceneAI::C_SaveGame::function(const C_DynArray<C_String>* arg)
{
	App->SaveGame("save_game.xml");
}

void::S_SceneAI::C_LoadGame::function(const C_DynArray<C_String>* arg)
{
	App->LoadGame("save_game.xml");
}

#pragma endregion