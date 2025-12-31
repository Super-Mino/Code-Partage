


#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <SDL3_GSlib-Sprite.hpp>
#include <SDL3_GSlib-ImGui.hpp>


#define WIN_W 1024
#define WIN_H 720



//===================================================================================




//===================================================================================














//===================================================================================


gs::Input input;


void Init(SDL_Window* win, SDL_Renderer* ren);
void InputGestion(bool& boolvar);
void Update(float dt, gs::Input& input, SDL_Window* win);
void Draw(SDL_Renderer* ren, float dt);
void GuiPart(SDL_Renderer* ren);


int main() 
{
	gs::Game game; 
	game.InitSDLEngine(WIN_W, WIN_H, "Sprite Animation Viewer - Sprite-Sheet", SDL_WINDOW_RESIZABLE);
	if(not game.isGameInit())
	{
		std::cout << "game.InitSDLEngine() failled\n";
		return 1;
	}
	
	if(not gs::InitImGui(game.win(), game.ren()))
		std::cout << "InitImGui() failled.\n";

	Init(game.win(), game.ren());
	
	
	bool go = true;
	while(go)
	{
		game.fpsProcessBegin();
		game.processDeltaTime();
		SDL_RenderClear(game.ren());
		
		InputGestion(go);
		
		Update(game.dt(), input, game.win());
		Draw(game.ren(), game.dt());
		
		{
			gs::ImGuiNewFrame();
			GuiPart(game.ren());
			gs::ImGuiRenderFrame(game.ren());
		}
		
		SDL_RenderPresent(game.ren());
		game.fpsProcessEnd(120); 
	
		gs::lastErrorAutomaticLog();
	}


	gs::QuitImGui();


	return 0; 
} 




//=========================================================================
//=========================================================================



void InputGestion(bool& boolvar)
{
	static SDL_Event event;
	while(SDL_PollEvent(&event))	
	{
		ImGui_ImplSDL3_ProcessEvent(&event);

		if(event.type == SDL_EVENT_QUIT) {
        	boolvar=false;  /* end the program, reporting success to the OS. */
    	}
		else
		if(event.type == SDL_EVENT_KEY_DOWN)
		{
			switch(event.key.key)
			{
			case SDLK_SPACE: input["space"] = true; break;
			case SDLK_S: input["s"] = true; break;
			default: break;
			}
				
		}
		else
		if(event.type == SDL_EVENT_KEY_UP)
		{
			switch(event.key.key)
			{
			case SDLK_SPACE: input["space"] = false; break;
			case SDLK_S: input["s"] = false; break;
			default: break;
			}
		}
		if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN and event.button.button == SDL_BUTTON_LEFT)
		{}
		else
		if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN and event.button.button == SDL_BUTTON_RIGHT)
		{}

		
	}

}



//=========================================================================
//=========================================================================


























