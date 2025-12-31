#include <SDL3_GSlib-Sprite.hpp>
#include "ImGui/imgui.h"


//===============================================================================================//

const size_t PATH_BUFF_SIZE=1500;
void SDLCALL fileDialogCallback(void *userdata, const char * const *filelist, int filter);

struct Params
{
	
	SDL_Renderer* ren = nullptr;
	gs::Vec2i frame_size{16,16};
	float frame_delay = 0.18f;
	gs::Sprite_Sheet spr_sheet;
	std::string loaded_spr_sheet_path;
	char path_buff[PATH_BUFF_SIZE];
	std::vector<int> chosen_anim_frames;
	
	void loadSpritesheet()
	{
		std::string path = std::string(path_buff);
		
		if(spr_sheet.load(ren, path, frame_size.to<unsigned short>(), frame_delay))
		{
			loaded_spr_sheet_path = path;
			chosen_anim_frames.clear();
		}
	}
	
	
	gs::Vec2i autoSize_helper__frames_count{0,0};
	void loadSpritesheet_autoSize()
	{
		std::string path = std::string(path_buff);
		
		if(autoSize_helper__frames_count == gs::Vec2i(0,0))
			return;
	
		if(spr_sheet.load(ren, path, gs::Vec2us(1, 1), frame_delay))
			loaded_spr_sheet_path = path;
		
		
		gs::Vec2f texture_size = spr_sheet.getTextureSize();
		
		if(texture_size.x != 0 and texture_size.y != 0)
		{
			gs::Vec2i frame_size = texture_size.to<int>() / autoSize_helper__frames_count;
			
			if(not spr_sheet.changeFrameSize(frame_size.to<unsigned short>()))
			{
				spr_sheet.unload();
				autoSize_helper__frames_count == gs::Vec2i(0,0);
				loaded_spr_sheet_path = "";
			}
		}
		else
		{
			spr_sheet.unload();
			loaded_spr_sheet_path = "";
		}
		
		chosen_anim_frames.clear();
	}
	
	
	void changeFrameSize()
	{
		if(spr_sheet.changeFrameSize(frame_size.to<unsigned short>()))
			chosen_anim_frames.clear();
		
	}
	
	
	void reload()
	{
		if(loaded_spr_sheet_path.size())
		{
			std::vector<int> animation = spr_sheet.getAnim();
		
			if(spr_sheet.load(ren, loaded_spr_sheet_path, spr_sheet.getFrameSize().to<unsigned short>(), spr_sheet.getFrameDelay()))
				spr_sheet.setAnim(animation);
		}
	
	}
	
	
	void openFileDialog()
	{
		
		if(file_explorer_state == READY)
			is_opening_file_screen = true;
		
	}
	
	
	enum File_Explo_State{READY, IN_PROCESS, ACHIEVED};
	File_Explo_State file_explorer_state=READY;
	std::string from_file_explorer;
	bool is_opening_file_screen=false;
	void update(SDL_Window* win, float dt)
	{
		spr_sheet.update(dt);
	
		if(is_opening_file_screen)
		{
			if(file_explorer_state == READY)
			{
				SDL_ShowOpenFileDialog(fileDialogCallback, //SDL_DialogFileCallback callback, 
								(void*)this, //void *userdata, 
								win, //SDL_Window *window, 
								nullptr, //const SDL_DialogFileFilter *filters, 
								0, //int nfilters, 
								nullptr, //const char *default_location, 
								false); //bool allow_many);
				
				file_explorer_state = IN_PROCESS;
			}
			else
			if(file_explorer_state == ACHIEVED)
			{	
				//Copie du chemin reçu dans le buffer.
				std::snprintf(path_buff, PATH_BUFF_SIZE, "%s", from_file_explorer.c_str());
				
				from_file_explorer="";
				file_explorer_state = READY;
				is_opening_file_screen = false;
			}
		}
	
	}
	
	
	void addFrameInAnim(int cell_num)
	{
		if(not spr_sheet.isLoaded() or cell_num < 0)
			return;
		
		//Vérification de la validité de l'indice reçu.
		gs::Vec2i frames_count = spr_sheet.getFramesCount();
		int max_cell_num = frames_count.x * frames_count.y -1;
		
		if(cell_num > max_cell_num)
			return;
			
		//Vérification de la présence. 
		//On va simplifier et considérer qu'une frame ne peut appairaître qu'une seule fois dans la séquence d'animation.
		//De ce fait si c'est déjà présent on va considérer que ceci est une demande de retrait.
		
		for(int i=0; i<chosen_anim_frames.size(); ++i)
			if(cell_num == chosen_anim_frames.at(i))
			{
				chosen_anim_frames.erase(chosen_anim_frames.begin()+i);
				return;
			}
			
		//Sinon on l'ajout à la suite des autres.
		chosen_anim_frames.push_back(cell_num);
	}
	
};

Params P;




//===============================================================================================//

void Init(SDL_Window* win, SDL_Renderer* ren)
{	
	SDL_SetRenderDrawColor(ren, 60u, 100u, 120u, 255u);
	
	P.ren = ren;
}


void Update(float dt, gs::Input& input, SDL_Window* win)
{
	P.update(win, dt);
}

void Draw(SDL_Renderer* ren, float dt)
{
	SDL_RenderTexture(ren, P.spr_sheet.getTexture(), nullptr, nullptr); //On met le sprite-sheet en décor de fond.
	gs::drawRect(ren, nullptr, {0,0,0,130}, true); //Et on l'assombri.
	
	//______________________________________________________________________
	
	int w_,h_; SDL_GetCurrentRenderOutputSize(ren, &w_, &h_);
	gs::Vec2f ren_size{static_cast<float>(w_), static_cast<float>(h_)};
	
	gs::Vec2f anim_pos{0.04882f, 0.25f};
	float anim_wh = (ren_size.y <= ren_size.x ? 0.375f : 0.26367f);
	
	
	
	float wh = (ren_size.y <= ren_size.x ? ren_size.y*anim_wh : ren_size.x*anim_wh);
	SDL_FRect dest{ren_size.x*anim_pos.x, ren_size.y*anim_pos.y, wh, wh}; //Pré-réglage au dimensions maximales choisies.
	
	if(P.spr_sheet.isLoaded())
	{
		gs::Vec2f frame_size = P.spr_sheet.getFrameSize();
		float frame_ratio = (frame_size.y=!0 and frame_size.x!=0 ? frame_size.x/frame_size.y : 1.f);
		
		
		if(frame_ratio >= 1.f) // aka w >= h
		{
			//On garde le w au max choisi, et on adapte le h pour (tenter de) conserver les proportions de la frame.
			dest.h = dest.w / frame_ratio;
		}
		else // aka h > w
		{
			//On garde le h au max choisi, et on adapte le w pour (tenter de) conserver les proportions de la frame.
			dest.w = dest.h * frame_ratio;
		}
	
		P.spr_sheet.drawCurrentFrame(ren, dest, 0.f, SDL_FLIP_NONE);
	}
		
		
	gs::drawRect(ren, dest, {240,240,240,230}, false);
	
}




ImVec2 operator+(const ImVec2& l, const ImVec2& r);// {return ImVec2(l.x+r.x, l.y+r.y);}
ImVec2 operator-(const ImVec2& l, const ImVec2& r);// {return ImVec2(l.x-r.x, l.y-r.y);}

void GuiPart(SDL_Renderer* ren)
{
	static float zoom = 1.f;
	static ImVec2 offset(0,0);
	
	if(zoom<=0) zoom = 1.f; 
	
	gs::Vec2f img_size = P.spr_sheet.getTextureSize();
	gs::Vec2f frame_size = P.spr_sheet.getFrameSize();
	gs::Vec2i frames_count = P.spr_sheet.getFramesCount();
	int w_,h_; SDL_GetCurrentRenderOutputSize(ren, &w_, &h_);
	gs::Vec2f ren_size{static_cast<float>(w_), static_cast<float>(h_)}; 
	
	
	ImGui::SetNextWindowPos(ImVec2(ren_size.x*0.37472f, ren_size.y*0.0125f));
	ImGui::SetNextWindowSize(ImVec2(ren_size.x*0.609375f, ren_size.y*0.6416));
	ImGui::Begin("Sprite-Sheet");
	ImGui::BeginChild("Img area", ImVec2(0,0), true, ImGuiWindowFlags_HorizontalScrollbar); //Création zone scrollable.
	
		if(ImGui::IsWindowHovered())
			zoom *= 1.f + ImGui::GetIO().MouseWheel * 0.1f;
			
		if(0)
		if(ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			offset.x += ImGui::GetIO().MouseDelta.x;
			offset.y += ImGui::GetIO().MouseDelta.y;
		}
			
			
		//Affichage de l'image.
		ImGui::SetCursorPos(offset);
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImGui::Image((ImTextureID)P.spr_sheet.getTexture(), ImVec2(img_size.x*zoom, img_size.y*zoom));
		
		
		
		//Dessin de la grille.
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 top_left = pos + offset;
		ImVec2 bottom_right = top_left + ImVec2(img_size.x*zoom, img_size.y*zoom);
		
		float cell_w = frame_size.x * zoom;
		float cell_h = frame_size.y * zoom;

		for(float x = top_left.x; x < bottom_right.x; x+=cell_w) //Lignes verticales
			draw_list->AddLine(ImVec2(x, top_left.y), ImVec2(x, bottom_right.y), IM_COL32(255,255,255,200));
		
		
		for(float y = top_left.y; y < bottom_right.y; y+=cell_h) //Lignes horizontales
			draw_list->AddLine(ImVec2(top_left.x, y), ImVec2(bottom_right.x, y), IM_COL32(255,255,255,200));
		
		
		//Mise à l'évidence des frames choisies dans la grille.
		auto highlightCell = [draw_list, cell_w, cell_h, top_left](float cell_x, float cell_y, int number_to_show) {
			
			ImVec2 rect_min = top_left + ImVec2(cell_x*cell_w, cell_y*cell_h);
			ImVec2 rect_max = rect_min + ImVec2(cell_w, cell_h);
		
			draw_list->AddRect(rect_min, rect_max, IM_COL32(0,200,0,255), 0.f, 0, 3.f);
			draw_list->AddText(rect_min+ImVec2(4,4), IM_COL32(230,230,230,255), std::to_string(number_to_show).c_str());
		};
		
		if(frames_count.x) //Sécurité
		for(int i=0; i<P.chosen_anim_frames.size(); ++i)
		{
			int cell_num = P.chosen_anim_frames.at(i);
			float cell_x = cell_num%frames_count.x;
			float cell_y = cell_num/frames_count.x;
			highlightCell(cell_x, cell_y, i); //En en effet on affiche l'indice de la frame dans l'animation et non le numéro de la cellule.
		}
			
		
		
		//Coo du pixel cliqué.
		if(ImGui::IsItemHovered() and ImGui::IsMouseClicked(0))
		{
			ImVec2 mouse_pos = ImGui::GetMousePos() - pos; //Position de la souris par rapport à l'image.
			ImVec2 pix_pos = ImVec2(mouse_pos.x/zoom, mouse_pos.y/zoom);
			
			//Selon la configuration, toute l'image n'est pas forcément comptabilisée pour composer la grille des frames (les cases non complètes selon le découpage ne sont pas prises en compte).
			gs::Vec2f real_used_area_size = frame_size*frames_count.to<float>(); 
			
			if(pix_pos.x < real_used_area_size.x and pix_pos.y < real_used_area_size.y) //On ne sélectionne que les cases dans la zone utile. 
			{
				float cx = int(pix_pos.x) / int(frame_size.x);
				float cy = int(pix_pos.y) / int(frame_size.y);
				
				float cell_num = cy*frames_count.x+cx;
				P.addFrameInAnim(cell_num);
			}
			
			
		}
		
		
	ImGui::EndChild();
	ImGui::End();
	
	
	ImGui::SetNextWindowPos(ImVec2(ren_size.x*0.21767f, ren_size.y*0.66388f));
	ImGui::SetNextWindowSize(ImVec2(ren_size.x*0.76679f, ren_size.y*0.32777f));
	ImGui::Begin("Parameters");
		
		if(ImGui::Button("file explorer"))
			P.openFileDialog();
		ImGui::SameLine();
		ImGui::InputText("sprite-sheet path", P.path_buff, PATH_BUFF_SIZE);
		
		ImGui::SliderFloat("frame delay", &P.frame_delay, 0.f, 3.f);
		P.spr_sheet.setFrameDelay(P.frame_delay);
		
	
		ImGui::InputInt2("frame Width x Height", &P.frame_size.x);
		
		
		if(ImGui::Button("Load with this frame size"))
			P.loadSpritesheet();
			
		if(P.spr_sheet.isLoaded())
		{
			ImGui::SameLine();
			if(ImGui::Button("Reload"))
				P.reload();
		}
			
		ImGui::SameLine();
		if(ImGui::Button("Change just frame size"))
			P.changeFrameSize();
			
		ImGui::Text("");
		ImGui::InputInt2("frame count : column x row", &P.autoSize_helper__frames_count.x);
		if(ImGui::Button("Load with this frames count"))
			P.loadSpritesheet_autoSize();
		if(P.spr_sheet.isLoaded())
		{
			ImGui::SameLine();
			if(ImGui::Button("Reload_"))
				P.reload();
		}
				
			
		
		ImGui::BeginChild("Img area", ImVec2(0,0), true, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::Text((std::string("last error : ") +gs::lastError()).c_str());
		ImGui::EndChild();

	ImGui::End();
	
	
	ImGui::SetNextWindowPos(ImVec2(ren_size.x*0.01757f, ren_size.y*0.0125f));
	ImGui::SetNextWindowSize(ImVec2(ren_size.x*0.3515625f, ren_size.y*0.2134f));
	ImGui::Begin("Info");
		ImGui::Text("Frame size       : %.0f x %.0f", frame_size.x, frame_size.y);
		ImGui::Text("Frame delay      : %.4f", P.spr_sheet.getFrameDelay());
		ImGui::Text("Spite-sheet size : %.0f x %.0f", img_size.x, img_size.y);
		ImGui::Text("zoom : %.4f", zoom);
		ImGui::SameLine();
		if(ImGui::Button("Reset zoom"))
			zoom = 1.f;
		
		ImGui::BeginChild("Img area", ImVec2(0,0), true, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::Text("Spite-sheet path : %s", P.loaded_spr_sheet_path.c_str());
		ImGui::EndChild();
		
	ImGui::End();
	
	
	ImGui::SetNextWindowPos(ImVec2(ren_size.x*0.01757f, ren_size.y*0.66388f));
	ImGui::SetNextWindowSize(ImVec2(ren_size.x*0.192625f, ren_size.y*0.32777f));
	ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
	static bool play_anim_once=false;
	
	if(P.spr_sheet.isLoaded())
	{
		if(ImGui::Button("Submit animation"))
			P.spr_sheet.setAnim(P.chosen_anim_frames);
			
		if(P.spr_sheet.animIsPlaying())
		{
			if(ImGui::Button("stop"))
				P.spr_sheet.stop();
		}
		else
		{
			if(ImGui::Button("play"))
				P.spr_sheet.start(play_anim_once);
			
			ImGui::SameLine();
			if(ImGui::Button("resume"))
				P.spr_sheet.resume();
		}
		
		ImGui::Checkbox("Play anim once", &play_anim_once);
	}
	else
		ImGui::Text("No sprite-sheet loaded yet.");
	
	ImGui::End();

}




void SDLCALL fileDialogCallback(void *userdata, const char * const *filelist, int filter)
{
	((Params*)userdata)->from_file_explorer = "";
	
	if(filelist)
	{
		int i = 0;
		while(filelist[i])
		{
			((Params*)userdata)->from_file_explorer = filelist[i]; 
			
			break; //++i;
		}
	}
	else
		std::cout << "File dialog error\n";
	
	
	((Params*)userdata)->file_explorer_state = Params::ACHIEVED;
}



