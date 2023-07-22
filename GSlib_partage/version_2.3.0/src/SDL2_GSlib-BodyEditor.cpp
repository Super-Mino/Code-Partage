#include "SDL2_GSlib-BodyEditor.hpp"




namespace sm
{

namespace EditBody
{




bool BodyEditor::init(SDL_Renderer* ren)
{
	std::ifstream file(save_file_path);

	if(not file.is_open())
	{
		std::ofstream newFile(save_file_path);
		file.open(save_file_path, std::ios::in);
	}
	if(not file.is_open())
	{
		sm::lastError(sm::str("||Warning|| in 'sm::EditBody::BodyEditor::init()' of '")+str_id+sm::str("', cannot open the file of backup. "));
		return false;
	}


	std::string id = std::string("#") + str_id;

	bool finding_id = true;
	while(finding_id)
	{
		std::string token;

		file >> token;

		if( token == id)
			finding_id = false; 

		if( file.eof() )
			break;
	}


	bool can_do = not finding_id;

	if(not can_do)
	{
		sm::lastError(sm::str("||Warning|| in 'sm::EditBody::BodyEditor::init()' of '")+str_id+sm::str("', ID not found. "));
		return true; //pour en créer un.
	}

	file >> img_path;
	file >> x_src;
	file >> y_src;
	file >> w_src;
	file >> h_src;

	file >> x_hitbox;
	file >> y_hitbox;
	file >> w_hitbox;
	file >> h_hitbox;

	file >> decalage_x_dest;
	file >> decalage_y_dest;
	file >> w_dest;
	file >> h_dest; 
	file >> angle_degre;

	if(not img_path.empty() and img_path != __No_Image_str__)
	{
		if(img_path.at(0) == '*') // = ce n'est pas un path mais le str_id d'un BodyEditor avec lequel partager la même texture
		{
			//recherche du BodyEditor
			std::string the_str_id = img_path.substr(1);	//on copie la chaine à partir de l indice 1 afin de ne pas prendre le '*'
			BodyEditor* the_bodyeditor_ptr = nullptr;
			
			for(BodyEditor& b : list_of_editors)
			{
				if(b.str_id == the_str_id)
				{
					the_bodyeditor_ptr = &b;
					break;
				}
			}

			if(the_bodyeditor_ptr != nullptr)
			{
				body->setTexture(the_bodyeditor_ptr->body->getTexture());
				texture_is_from_file=false;
			}
			else
			{
				sm::lastError(sm::str("||Warning|| in 'sm::EditBody::BodyEditor::init()' of '")+str_id+sm::str("', [maybe called by 'sm::EditBody::addEditor()'], cannot find the BodyEditor '")+the_str_id+sm::str("' to get the texture. "));
			}
			
		}
		else
		if(ren)
		{
			bool ok = body->setImg(ren, img_path.c_str());

			if(not ok)
			{
				sm::lastError(sm::str("from 'sm::EditBody::BodyEditor::init()' of '")+str_id+sm::str("' [maybe called by 'sm::EditBody::addEditor()'] : ")+sm::lastError());
				//pas de return ici car ne perturbe pas le reste
			}
		}
	}

	

	body->setSrcX(x_src); 
	body->setSrcY(y_src); 
	body->setSrcW(w_src); 
	body->setSrcH(h_src); 

	body->initHitbox(x_hitbox, y_hitbox, w_hitbox, h_hitbox);

	body->setDestRectDecalage(decalage_x_dest, decalage_y_dest); 
	body->setDestW(w_dest); 
	body->setDestH(h_dest);
	
	body->setAngle(sm::rad(angle_degre));


	return true;
}










void BodyEditor::guiEditor(Game* game)
{
	SDL_Renderer* ren = nullptr;

	if(game != nullptr)
		ren = game->ren();

	ImGui::Begin(str_id.c_str());

		ImGui::SetWindowSize(ImVec2(365, 410), ImGuiCond_FirstUseEver); 

		ImGui::Checkbox("Save changes at end", &save_changes_at_end);

		// static char img_path_buf[245] = "";
			// ImGui::InputText("file path", img_path_buf, IM_ARRAYSIZE(img_path_buf));
			
			// if(ImGui::Button("load image"))
			// {
			// 	SDL_Renderer* ren = nullptr;
			// 	ren = ThisGame->ren();
			// 	img_path = img_path_buf;
			// 	body->setImg(ren, img_path.c_str());
		// }

		ImGui::SeparatorText("About texture is below (at the end)");
		ImGui::SeparatorText("");


		
		ImGui::InputInt("x_src", &x_src); 
		ImGui::InputInt("y_src", &y_src); 
		ImGui::InputInt("w_src", &w_src); 
		ImGui::InputInt("h_src", &h_src); 

		ImGui::InputInt("x_hitbox", &x_hitbox);
		ImGui::InputInt("y_hitbox", &y_hitbox);
		ImGui::InputInt("w_hitbox", &w_hitbox);
		ImGui::InputInt("h_hitbox", &h_hitbox);

		ImGui::InputInt("decalage_x_dest", &decalage_x_dest);
		ImGui::InputInt("decalage_y_dest", &decalage_y_dest);
		ImGui::InputInt("w_dest", &w_dest);
		ImGui::InputInt("h_dest", &h_dest);
		ImGui::SliderFloat("angle (degré) sens 1", &angle_degre, 0.f, 360.f);
		ImGui::SliderFloat("angle (degré) sens 2", &angle_degre, -360.f, 0.f);


		en_attendant__pour_la_partie_de_la_texture:
		{
			ImGui::Text("");
			ImGui::SeparatorText("About texture");

			ImGui::Text("");

			if(texture_is_from_file)
				ImGui::Text("|Naw texture is load from \"%s\" file.", img_path.c_str());
			else
				ImGui::Text("|Naw texture is set from \"%s\" editor.", img_path.substr(1).c_str());

			ImGui::Text("|You can change the provenance below.");

			if(not list_of_editors.empty())
				ImGui::Text("|[load from file or set the same as : ]");
			
			ImGui::Text("");
				
				
				// m_a=0;
				ImGui::RadioButton("->_Load_From_File_<-", (int*)&m_a, 0);
				
				for(int i=0; i< list_of_editors.size(); i++)
				{
					BodyEditor& be = list_of_editors.at(i);
					ImGui::RadioButton((sm::str("-> ") + be.str_id).c_str(), (int*)&m_a, i+1);
				}

				
				if(m_a == 0)
				{
					m_set_or_load_str = "load texture";
					ImGui::InputText("file path", m_img_path_buf, IM_ARRAYSIZE(m_img_path_buf));
				}
				else
					m_set_or_load_str = "set texture";

				

				if(ImGui::Button(m_set_or_load_str.c_str()))	//boutton a cliquer pour charger ou associer la texture demandée
				{
					bool previous_texture_modif_is_from_file = texture_is_from_file;

					if(m_a > 0)
					{
						unsigned int index = m_a-1;
						BodyEditor& be = list_of_editors.at(index);
						body->setTexture(be.body->getTexture());
						texture_is_from_file=false;
						img_path = std::string("*") + be.str_id;
					}
					else
					if(ren)
					{
						img_path = m_img_path_buf;
						bool destroy_previous = previous_texture_modif_is_from_file;

						bool ok = body->setImg(ren, img_path.c_str(), destroy_previous);
						texture_is_from_file=true;	//même si ça n'a pas pu load c'est quand même from file
						

						if(not ok)
						{
							sm::lastError(sm::str("from 'sm::EditBody::BodyEditor::guiEditor()' of '")+str_id+sm::str("' [maybe called by 'sm::EditBody::launcheEditors()'] : ")+sm::lastError());
							//pas de return ici car ne perturbe pas le reste
						}
							
					}
					else
						sm::lastError(sm::str("||Warning|| in 'sm::EditBody::BodyEditor::guiEditor()' of '")+str_id+sm::str("' [maybe called by 'sm::EditBody::launcheEditors()'], cannot load the texture : invalid renderer. "));
					
				}

				// static int a=0;
				// ImGui::RadioButton("z", &a, 0);
				// ImGui::RadioButton("z", &a, 1);

			ImGui::SeparatorText("");
		}
		

	ImGui::End();



	//update changes
	
	body->setSrcX(x_src); 
	body->setSrcY(y_src); 
	body->setSrcW(w_src); 
	body->setSrcH(h_src); 

	body->initHitbox(x_hitbox, y_hitbox, w_hitbox, h_hitbox);
	
	body->setDestRectDecalage(decalage_x_dest, decalage_y_dest); 
	body->setDestW(w_dest); 
	body->setDestH(h_dest);
	
	body->setAngle(sm::rad(angle_degre));
	
}




























//===================================================================================//




bool addEditor(sm::Body& body, std::string id, Game* game) 
{
	//Afin de ne pas avoir deux id identiques. 
		//Note : comme ImGui et std::string sont sensibles à la casse on peut se permttre par exemple d'avoir un id "Player" et un autre "player" (heureusement) ;)
	{
		bool already_in=false;

		for(BodyEditor& bd : list_of_editors)
		{
			if(id == bd.str_id)
			{
				already_in = true;
				break;
			}
		}

		if(already_in)
		{
			sm::lastError(sm::str("||Warning|| in 'sm::EditBody::addEditor()', the id '")+id+sm::str("' is already used, cannot add it. [try to change it] "));
			return false;
		}
	}


	SDL_Renderer* ren=nullptr;

	if(game != nullptr)
	{
		ren = game->ren();
	}
	else
		if(ThisGame != nullptr)
			ren = ThisGame->ren();
		

	if(ren == nullptr)
	{
		sm::lastError(sm::str("||Warning|| in 'sm::EditBody::addEditor()' with the ID '")+id+sm::str("', invalid renderer. "));
		return false;
	}


	BodyEditor tmp;
	list_of_editors.push_back(tmp); 
	BodyEditor& init = list_of_editors.back(); 


	init.body = &body;
	init.str_id = id;


	bool ok = init.init(ren);
	
	if(not ok)
	{
		list_of_editors.pop_back();
		sm::lastError(sm::str("from 'sm::EditBody::addEditor()' : ")+sm::lastError());
		return false;
	}

	return true;
}









void launcheEditors(Game* game)
{
	for ( auto& init : list_of_editors )
	{
		init.guiEditor(game);
	}
}








void save()
{
	if(not save_changes_at_end)
		return;

	std::ofstream file(save_file_path, std::ios::out); 

	for( auto& init : list_of_editors)
	{		
		std::string id = std::string("#") + init.str_id;

		file << id << "\n";

		if(init.img_path.empty())
		{
			file << __No_Image_str__;
			file << "\n";
		}
		else
		{	
			file << init.img_path; 
			file << "\n";
		}
		

		file << init.x_src; file << " ";
		file << init.y_src; file << " ";
		file << init.w_src; file << " ";
		file << init.h_src; file << "\n";

		file << init.x_hitbox; file << " ";
		file << init.y_hitbox; file << " ";
		file << init.w_hitbox; file << " ";
		file << init.h_hitbox; file << "\n";

		file << init.decalage_x_dest; file << " ";
		file << init.decalage_y_dest; file << " ";
		file << init.w_dest; file << " ";
		file << init.h_dest; file << " ";
		file << init.angle_degre; file << "\n\n";
	}

}

}



























//===================================================================================//



bool InitImGui(SDL_Window* win, SDL_Renderer* ren, Game* g)
{
	if(g == nullptr)
		g = ThisGame;


	if( win == nullptr )
	{
		if(g == nullptr)
			return false;

		win = g->win();
		if( win == nullptr )
			return false;
	}

	if( ren == nullptr )
	{
		if(g == nullptr)
			return false;

		ren = g->ren();
		if( ren == nullptr )
			return false;
	}

	if(ren and win)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplSDL2_InitForSDLRenderer(win, ren);
		ImGui_ImplSDLRenderer_Init(ren);
	}
	else
		return false;

	return true;

}






void ImGuiNewFrame()
{
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}






void RenderTheImGuiFrame()
{
	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}






void QuitImGui()
{
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}


} //fin namespace sm

