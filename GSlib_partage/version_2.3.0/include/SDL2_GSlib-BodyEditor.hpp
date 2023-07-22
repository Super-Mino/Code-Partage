#ifndef _SDL2_GSlib_BODY_EDITOR_HPP_
#define _SDL2_GSlib_BODY_EDITOR_HPP_
//version modifiée le 14/07/23 (à moins d'oubli de modifier cette ligne).


/*

l'ordre des paramètres dans le .ini 

path de l image ou str_id (mais commençant par un '*') de celui avec qui on doit tenir la texture (il est important alors que celui de qui on va tenir la texture est un body editor initialisé avant)
x src
y src
w src
h src
x hitbox
y hitbox
w hitbox
h hitbox
décalage x de dest
décalege y de dest
w dest
h dest
angle de rotation



*/


#include "SDL2_GSlib-Entity.hpp"
#include "fstream"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"



namespace sm
{

namespace EditBody
{

struct BodyEditor
{
	std::string 	img_path="";
	sm::Body* 		body;
	std::string 	str_id;
	bool 			texture_is_from_file=true; //sinon "is from other body"
	unsigned int 	m_a=0;	//permet de connaitre le choix courant dans le radio button
	char 			m_img_path_buf[1024]="";
	std::string 	m_set_or_load_str="set texture";


	int x_src = 0;
	int y_src = 0;
	int w_src = 0;
	int h_src = 0;

	int x_hitbox = 0;
	int y_hitbox = 0;
	int w_hitbox = 0;
	int h_hitbox = 0;

	int decalage_x_dest = 0;
	int decalage_y_dest = 0;
	int w_dest = 0;
	int h_dest = 0; 
	float angle_degre = 0;

	bool init(SDL_Renderer* ren);
	void guiEditor(Game* game);
};




static std::string save_file_path = "BodyEditor.ini"; 

static std::string __No_Image_str__ = "No_Image";

static std::vector<BodyEditor> list_of_editors; 

static bool save_changes_at_end=true;


bool addEditor(sm::Body& body, std::string id, Game* game=nullptr);

void launcheEditors(Game* game);

void save();


}











bool InitImGui(SDL_Window* win=nullptr, SDL_Renderer* ren=nullptr, Game* g=nullptr);
void ImGuiNewFrame();
void RenderTheImGuiFrame();
void QuitImGui();

}







#endif 