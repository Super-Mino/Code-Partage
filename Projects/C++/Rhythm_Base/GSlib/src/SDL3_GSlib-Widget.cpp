#include "SDL3_GSlib-Widget.hpp"
#include "SDL3_GSlib-Geo.hpp"


namespace gs
{

struct Vec2Texture
{
	Vec2Texture() = default;
		SDL_Texture* active=nullptr, * idle=nullptr;
	~Vec2Texture() {}
};

// void INTERNAL_USE_addButton(Button& button, Uint16& id_ref);

// void INTERNAL_USE_editState(Button& btn, wi::Button_State state, bool new_state);

// void INTERNAL_USE_addButton(Button& button, Uint16& id_ref);

// void INTERNAL_USE_initButtonFont(SDL_Renderer* ren, size_t pt_size);

// std::string INTERNAL_USE_switchText(Button& btn, std::string to_witch);





gs::Game*                                  game_ptr=nullptr;
bool                                       widgets_init=false;

//std::unordered_set<Button*>                buttons_set;
//std::unordered_set<Scrollbar*>             scrollbars_set;
std::vector<Button*>                       buttons_vec;
std::vector<Scrollbar*>                    scrollbars_vec;

gs::Font                                   buttons_font;
bool                                       buttons_font_is_init=false;
std::string                                buttons_font_path="C:/Windows/boot/Fonts/Meiryo_boot.ttf";

std::unordered_map<Uint16, Vec2Texture>    buttons_text_textures;
Uint16 	                                   buttons_curr_free_id=0;



void INTERNAL_USE_editState(Button& btn, wi::Button_State state, bool new_state)
{
	if(!btn.m_is_init)
		return;

	switch(state)
	{
		case wi::MOUSE_IN: btn.m_mouse_in = new_state;
			break;

		case wi::CLICKED_IN: 
			btn.m_clicked_in = new_state;
			
			if(new_state == true)
			{
				bool previous_state = btn.m_active_state;
				btn.m_active_state = !btn.m_active_state;

				if(previous_state != btn.m_active_state)
					if(btn.m_callback)
					{
						btn.m_last_callback_return_val = btn.m_callback(btn.m_callback_arg);
					}

				if(btn.m_switch_text_by_state)
				{
					if(btn.m_active_state == false)
						btn.switchText("idle");
					else
						btn.switchText("active");
				}
			}

			break;

		default:return;
	}
}



void INTERNAL_USE_addButton(Button& button, Uint16& id_ref)
{
	id_ref = buttons_curr_free_id;
	buttons_curr_free_id++;

	//buttons_set.insert(&button);
	buttons_vec.push_back(&button);
}



void INTERNAL_USE_initButtonFont(SDL_Renderer* ren, size_t pt_size)
{	
	if(buttons_font_is_init) return;

	bool res = buttons_font.init(ren, buttons_font_path.c_str(),
											pt_size, gs::te::BLENDED);

	if(not res)
	{
		gs::lastError(gs::str("from 'INTERNAL_USE_initButtonFont()' : ")+gs::lastError());
		return;
	}

	buttons_font_is_init = true;
}



std::string INTERNAL_USE_switchText(Button& btn, std::string to_witch)
{
	if(buttons_text_textures.contains(btn.m_button_id))
	{
		if(to_witch == "active")
			btn.m_text_texture = buttons_text_textures.at(btn.m_button_id).active;

		else
		if(to_witch == "idle")
			btn.m_text_texture = buttons_text_textures.at(btn.m_button_id).idle;

		else
		{
			if(btn.m_text_texture == buttons_text_textures.at(btn.m_button_id).active)
				btn.m_text_texture = buttons_text_textures.at(btn.m_button_id).idle;
			else
				btn.m_text_texture = buttons_text_textures.at(btn.m_button_id).active;
		}
	}
	else
		return std::string("this button doesn't have a text manager. [use 'setText(Button)' to init one. (given button id : ")+std::to_string(btn.m_button_id)+gs::str(")]. ");

	return std::string();
}



bool initWidgets(gs::Game* g, std::string font_path)
{
	if(g)
	{
		game_ptr = g;
		buttons_font_path = font_path;
		widgets_init = true;

		return true;
	}
	
	gs::lastError("In 'initWidgets()', given gs::Game is invalid. ");
	return false;
}


void widgetsProcessEvent(SDL_Event& ev)
{
	static SDL_FPoint mouse_curr_pos;
	
	SDL_GetMouseState(&mouse_curr_pos.x, &mouse_curr_pos.y);
	// mouse_curr_pos.x = ev.motion.x; 
	// mouse_curr_pos.y = ev.motion.y; 

	//for(Button* btn : buttons_set)
	for(Button* btn : buttons_vec)	
	{
		if(not btn)
			continue;
		
		SDL_Rect btn_area = rect(btn->getArea());
		SDL_Point mouse_curr_pos_i = SDL_Point(static_cast<int>(mouse_curr_pos.x), static_cast<int>(mouse_curr_pos.y));

		if(SDL_PointInRect(&mouse_curr_pos_i, &btn_area))
		{
			INTERNAL_USE_editState(*btn, wi::MOUSE_IN, true);

			if(ev.type == btn->getTargetEvent())
			{
				if(ev.button.button == btn->getTargetButtonEvent())
					INTERNAL_USE_editState(*btn, wi::CLICKED_IN, true);
				else
					INTERNAL_USE_editState(*btn, wi::CLICKED_IN, false);
			}
			else
				INTERNAL_USE_editState(*btn, wi::CLICKED_IN, false);
				
		}
		else
		{
			INTERNAL_USE_editState(*btn, wi::MOUSE_IN, false);
			INTERNAL_USE_editState(*btn, wi::CLICKED_IN, false);
		}
	}

	//for(Scrollbar* bar : scrollbars_set)
	for(Scrollbar* bar : scrollbars_vec)
		if(bar)
			bar->processEvent(ev);

}


void updateWidgets()
{
	//for(Scrollbar* bar : scrollbars_set)
	for(Scrollbar* bar : scrollbars_vec)
		if(bar)
			bar->update();
}




bool setButtonText(Button& btn, std::string witch, const char* text, SDL_Color text_color, bool resize_area_w)
{
	if(not widgets_init)
	{
		gs::lastError("In 'setButtonText()', widgets not yet init, cannot use font. [Use 'gs::initWidgets()' before]. ");
		return false;
	}

	bool resize_font = true;
	if(not buttons_font_is_init)
	{
		INTERNAL_USE_initButtonFont(game_ptr->ren(), 20);//btn.m_area.h);
		bool resize_font = false;
	}

	if(not buttons_font_is_init) 
	{
		// gs::lastError("In 'setButtonText(Button)', button font is not init (init failled). ");
		gs::lastError(gs::str("from 'setButtonText(Button)' : ")+gs::lastError());
		return false;
	}

	if(not btn.m_is_init)
	{
		gs::lastError("In 'setButtonText(Button)', button is not yet init. ");
		return false;
	}	

	if(not buttons_text_textures.contains(btn.m_button_id))
	{
		auto res = buttons_text_textures.insert({btn.m_button_id, Vec2Texture()});
		if(not std::get<1>(res))
		{
			gs::lastError("In 'setButtonText(Button)', insert text texture manager failled. ");
			return false;
		}
	}

	Vec2Texture& vec2T = buttons_text_textures.at(btn.m_button_id);

	if(resize_font)
		TTF_SetFontSize(buttons_font.getFont(), btn.m_area.h);

	SDL_Rect text_size;

	SDL_Texture* rendered_text = buttons_font.render(text, text_color, 
															te::SELF_DEFAULT, nullptr, &text_size);
	if(!rendered_text)
	{
		gs::lastError(gs::str("In 'setText(Button)', render text failled : ")+gs::lastError());
		return false;
	}
	


	if(witch == "active")
	{
		if(vec2T.active)
			SDL_DestroyTexture(vec2T.active);
	

		vec2T.active = rendered_text;
		btn.m_text_texture = vec2T.active;
	}
	else
	if(witch == "idle")
	{
		if(vec2T.idle)
			SDL_DestroyTexture(vec2T.idle);

		vec2T.idle = rendered_text;
		btn.m_text_texture = vec2T.idle;
	}
	else
	{
		SDL_DestroyTexture(rendered_text);
		gs::lastError(gs::str("In 'setText(Button)', witch text texture to work with ? ['active' or 'idle', given : '")+witch+gs::str("']. "));
		return false;
	}

	if(resize_area_w)
	{
		text_size.x = btn.m_area.x;
		text_size.y = btn.m_area.y;
		text_size.h = btn.m_area.h;
		btn.setArea(frect(text_size));
	}

	gs::addToClearAtTheEnd(rendered_text);

	btn.m_show_text=true;
	return true;
}








//==============================================================================//
//==============================================================================//
//==============================================================================//


Text_Assembly::~Text_Assembly()
{
	int i=0;
	for(auto& rendered_one : rendered_ones)
	{
		SDL_Texture* tex = std::get<1>(rendered_one).texture;
		if(tex)
			SDL_DestroyTexture(tex);

		i++;
	}

};


void Text_Assembly::init(gs::Font* _font)
{font = _font;}


bool Text_Assembly::initTextTextureBuff(SDL_Renderer* ren, SDL_Texture** tex, gs::Vec2i size)
{
	*tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_TARGET , size.x, size.y);

	if(!(*tex))
	{
		std::cout << "\n-> Text_Assembly::initTextTextureBuff() : SDL_CreateTexture() failled : "
			<< SDL_GetError() << "\n"; 
		return false;
	}

	SDL_SetTextureBlendMode(*tex, SDL_BLENDMODE_BLEND);
	return tex;
}


bool Text_Assembly::addToken(std::string token)
{
	if(not font or rendered_ones.contains(token))
		return false;

	SDL_Rect size;
	Text_Token tk;
	tk.texture = font->render(token.c_str(), text_color, te::SELF_DEFAULT, nullptr, &size);

	if(not tk.texture)
	{
		std::cout << "\n-> Text_Assembly::addToken() : " << gs::lastError() << "\n";
		return false;
	}

	tk.size.x = size.w;
	tk.size.y = size.h;

	rendered_ones[token] = tk;
	return true;
}



int Text_Assembly::assemble(SDL_Renderer* ren, SDL_Texture* target_texture, std::vector<std::string> tokens)
{
	//Vérification générale des info.
	if(not target_texture)
		return -1;
	
	Sint64 default_value = -1;
	int texture_access = SDL_GetNumberProperty(SDL_GetTextureProperties(target_texture), "SDL_PROP_TEXTURE_ACCESS_NUMBER", default_value);
	

	if(texture_access == default_value or texture_access != SDL_TEXTUREACCESS_TARGET)
		return -2;

	//Préparation de texture cible afin d'y écrire (dessiner les tokens).
	SDL_Texture* previous_target = SDL_GetRenderTarget(ren);
	SDL_SetRenderTarget(ren, target_texture);

	//Nettoyage de la texture cible.
	SDL_Color previousC;
    SDL_GetRenderDrawColor(ren, &previousC.r,&previousC.g,&previousC.b,&previousC.a);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0); //C'est le dernier zéro qui compte.
   	SDL_RenderFillRect(ren, nullptr);
    SDL_SetRenderDrawColor(ren, previousC.r,previousC.g,previousC.b,previousC.a);


    //Parcour + dessin des différents tokens.
	bool error_is_printed = false;
	int return_val = 0; // = success
	short int current_x = 0;

	for(std::string& token : tokens)
	{
		if(rendered_ones.contains(token))
		{
			render_here:

			Text_Token& text_token = rendered_ones.at(token);

			SDL_Texture* curr_token_texture;
			curr_token_texture = text_token.texture;

			SDL_FRect dest_rect = {static_cast<float>(current_x), 0.f, 
									static_cast<float>(text_token.size.x), static_cast<float>(text_token.size.y)};
			
			bool res = SDL_RenderTexture(ren, curr_token_texture, nullptr, &dest_rect);
			if(!res)
			{
				if(not error_is_printed)
				{	
					std::cout << "\n-> Text_Assembly::assemble() : at token '" << token 
					<<"', error : " << SDL_GetError() << "\n";
					error_is_printed = true;
				}

				return_val = -3;
			}

			current_x += dest_rect.w;
		}
		else
		{
			if(addToken(token))
				goto render_here;
		}
	}

	SDL_SetRenderTarget(ren, previous_target);
	return return_val;
}




















//==============================================================================//
//==============================================================================//
//==============================================================================//



Button::Button() 
{
	INTERNAL_USE_addButton(*this, m_button_id);
};

Button::~Button()
{
	// removeButton ?
}



bool Button::init(Game* g, SDL_FRect area, const char* img_file_path)
{
	if(m_is_init)
	{
		gs::lastError("in 'Button::init()', button already init. ");
		return false;
	}

	if(area.w <= 0 or area.h <= 0)
	{
		gs::lastError("in 'Button::init()', given area is invalid (negative size or empty rect). ");
		return false;
	}

	m_area = area;

	m_texture_dest_size.x = m_area.w;
	m_texture_dest_size.y = m_area.h;

	m_text_dest_size.x = m_area.w;
	m_text_dest_size.y = m_area.h;

	bool success = true;

	if(img_file_path)
	if(g)
	{
		m_texture = g->loadImg(img_file_path);

		if(not m_texture)
		{
			gs::lastError(gs::str("from 'Button::init()' : ")+gs::lastError());
			success = false;
		}
		else
		{
			m_show_img = true;
			gs::addToClearAtTheEnd(m_texture);
		}
	}
	
		
	m_is_init = true;
	return success;
}

bool Button::loadTexture(Game* g, const char* img_file_path)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::loadTexture()', button not yet init. ");
		return false;
	}

	if(not g)
	{
		gs::lastError("In 'Button::loadTexture()', given gs::Game* is invalid. ");
		return false;
	}

	m_texture = g->loadImg(img_file_path);

	if(not m_texture)
	{
		gs::lastError(gs::str("from 'Button::init()' : ")+gs::lastError());
		return false;
	}
	else
	{
		m_show_img = true;
		gs::addToClearAtTheEnd(m_texture);
	}
		

	return true;
}

bool Button::setTexture(SDL_Texture* texture)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::setTexture()', button not yet init. ");
		return false;
	}

	if(texture)
	{
		gs::addToClearAtTheEnd(texture);
		m_texture = texture;
		m_show_img = true;
		return true;
	}
	else
		gs::lastError("In 'Button::setTexture()', given texture is invalid. ");

	return false;
}

bool Button::show(Game* g)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::show()', button not yet init. ");
		return false;
	}

	if(not g)
	{
		gs::lastError("In 'Button::show()', given gs::Game is invalid. ");
		return false;
	}
	
	bool success = true;
	SDL_BlendMode blend_mode = SDL_BLENDMODE_BLEND;
	
	if(m_show_area)
	{
		SDL_FColor curr_col;
		
		if(m_active_state)
			curr_col = m_active_color;

		else
			curr_col = m_idle_color;

		gs::drawRect(g->ren(), &m_area, curr_col, true, &blend_mode);

		if(m_clicked_in)
			gs::drawRect(g->ren(), &m_area, m_clicked_in_color, true, &blend_mode);;

		
		
	}

	if(m_show_img)
	{
		if(m_texture)
		{
			SDL_FRect dest_rect={
				m_area.x+m_texture_offset.x,
				m_area.y+m_texture_offset.y,
				static_cast<float>(m_texture_dest_size.x),
				static_cast<float>(m_texture_dest_size.y)
			};

			bool res = SDL_RenderTexture(g->ren(), m_texture, nullptr, &dest_rect);

			if(!res)
			{
				gs::lastError(gs::str("In 'Button::show()', show img failled : ")+gs::str(SDL_GetError()));
				success = false;
			}
		}
	}



	if(m_show_text)
	{
		SDL_FRect dest_rect={
			m_area.x+m_text_offset.x,
			m_area.y+m_text_offset.y,
			static_cast<float>(m_text_dest_size.x),
			static_cast<float>(m_text_dest_size.y)
		};

		bool res = SDL_RenderTexture(g->ren(), m_text_texture, nullptr, &dest_rect);

		if(!res)
		{
			gs::lastError(gs::str("In 'Button::show()', show text failled : ")+gs::str(SDL_GetError()));
			success = false;
		}
	}


	if(m_mouse_in)
		gs::drawRect(g->ren(), &m_area, m_mouse_in_color, true, &blend_mode);

	if(m_clicked_in)
		gs::drawRect(g->ren(), &m_area, m_clicked_in_color, false);
		//Car l'image peut avoir caché la représentation graphique du "clicked in".

	return success;
}

bool Button::setShow(wi::Button_Field field, bool new_state)
{
	bool previous_state;

	if(field == wi::AREA)
	{
		previous_state = m_show_area;
		m_show_area = new_state;
		return previous_state;
	}
	else
	if(field == wi::IMG)
	{
		previous_state = m_show_img;
		m_show_img = new_state;
		return previous_state;
	}
	else
	if(field == wi::TEXT)
	{
		previous_state = m_show_text;
		m_show_text = new_state;
		return previous_state;
	}

	return false;
}

void Button::setArea(SDL_FRect new_area)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::setArea()', button not yet init. ");
		return;
	}

	m_area = new_area;

	m_texture_dest_size.x = m_area.w;
	m_texture_dest_size.y = m_area.h;

	m_text_dest_size.x = m_area.w;
	m_text_dest_size.y = m_area.h;
}

void Button::incrArea(short x, short y, short w, short h)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::incrArea()', button not yet init. ");
		return;
	}

	m_area.x += x;
	m_area.y += y;
	m_area.w += w;
	m_area.h += h;

	m_texture_dest_size.x = m_area.w;
	m_texture_dest_size.y = m_area.h;

	m_text_dest_size.x = m_area.w;
	m_text_dest_size.y = m_area.h;
}

void Button::setActiveState(bool new_state)
{
	if(not m_is_init)
	{
		//gs::lastError
		return;
	}

	m_active_state = new_state;
}

void Button::incrOffset(wi::Button_Field field, short x, short y)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::incrOffset()', button not yet init. ");
		return;
	}

	if(field == wi::IMG)
	{
		m_texture_offset.x += x;
		m_texture_offset.y += y;
	}
	else
	if(field == wi::TEXT)
	{
		m_text_offset.x += x;
		m_text_offset.y += y;
	}
}

void Button::setDestSize(wi::Button_Field field, short w, short h)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::setDestSize()', button not yet init. ");
		return;
	}

	if(field == wi::IMG)
	{
		m_texture_dest_size.x = w;
		m_texture_dest_size.y = h;
	}
	else
	if(field == wi::TEXT)
	{
		m_text_dest_size.x = w;
		m_text_dest_size.y = h;
	}
		
}

void Button::setColor(wi::Button_State target_state, SDL_FColor c)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::setColor()', button not yet init. ");
		return;
	}

	switch(target_state)
	{
		case wi::IDLE: m_idle_color = c; break;
		case wi::ACTIVE: m_active_color = c; break;
		case wi::MOUSE_IN: m_mouse_in_color = c; break;
		case wi::CLICKED_IN: m_clicked_in_color = c; break;
		
		default:
			break;
	}
}

void Button::setTargetEvent(Uint32 target_event)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::setTargetEvent()', button not yet init. ");
		return;
	}

	m_target_action = (wi::Action_Val)target_event;
}


void Button::setTargetButtonEvent(Uint8 id)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::setTargetButtonEvent()', button not yet init. ");
		return;
	}

	m_target_button_id = id;
}

bool Button::setCallbackOfTargetEvent(int (*callback)(void*), void* arg)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::setCallbackOfTargetEvent()', button not yet init. ");
		return false;
	}

	m_callback = callback;
	m_callback_arg = arg;
	return true;
}


bool Button::switchText(std::string to_witch) //Avec 'to_witch' != 'active' et de 'idle', ça switch sur l'autre qui n'est pas actuellement. 
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::switchText()', button not yet init. ");
		return false;
	}

	std::string res = INTERNAL_USE_switchText(*this, to_witch);

	if(not res.empty())
	{
		gs::lastError(gs::str("In 'Button::switchText()', ")+res);
		return false;
	}

	return true;
}

bool Button::setSwitchTextByState(bool new_state)
{
	if(not m_is_init)
	{
		gs::lastError("In 'Button::setSwitchTextByState()', button not yet init. ");
		return false;
	}

	m_switch_text_by_state = new_state;
	return true;
}


bool Button::isInit() const
{return m_is_init;}

SDL_Texture* Button::getTexture()
{return m_texture;}

bool Button::getShow(wi::Button_Field field)
{
	if(field == wi::AREA)
		return m_show_area;
	else
	if(field == wi::IMG)
		return m_show_img;
	else
	if(field == wi::TEXT)
		return m_show_text;

	return false;
}

SDL_FRect Button::getArea() const
{return m_area;}

bool Button::getActiveState() const
{return m_active_state;}

Vec2i Button::getOffset(wi::Button_Field field) const
{
	if(field == wi::IMG)
		return m_texture_offset;

	if(field == wi::TEXT)
		return m_text_offset; 
	return {0,0};
}

Vec2i Button::getDestSize(wi::Button_Field field) const
{
	if(field == wi::IMG)
		return m_texture_dest_size;

	if(field == wi::TEXT)
		return m_text_dest_size;

	return {static_cast<int>(m_area.w), static_cast<int>(m_area.h)};
}

SDL_FColor Button::getColor(wi::Button_State target_state) const
{
	switch(target_state)
	{
		case wi::IDLE: return m_idle_color; 
		case wi::ACTIVE: return m_active_color; 
		case wi::MOUSE_IN: return m_mouse_in_color; 
		case wi::CLICKED_IN: return m_clicked_in_color; 
		
		default: 
			break;
	}

	return {0,0,0,0};
}

bool Button::mouseIn() const
{return m_mouse_in;}

bool Button::clickedIn() const
{return m_clicked_in;}

Uint32 Button::getTargetEvent()
{return m_target_action;}

Uint8 Button::getTargetButtonEvent()
{return m_target_button_id;}

int Button::getLastCallbackReturnVal()
{return m_last_callback_return_val;}

bool Button::getSwitchTextByState()
{return m_switch_text_by_state;}
















//==============================================================================//
//==============================================================================//
//==============================================================================//

Scrollbar::Scrollbar()
{
	try{
		
	//scrollbars_set.insert(this);
	scrollbars_vec.push_back(this);
	
	}catch(std::exception& excep){gs::lastError("excep in Scrollbar::Scrollbar() : ",excep.what(),"\n");}
}

Scrollbar::~Scrollbar()
{
	//scrollbars_set.erase(this);
	for(int i=0; i<scrollbars_vec.size(); ++i)
	{
		Scrollbar* bar = scrollbars_vec.at(i);
		if(bar == this)
		{
			scrollbars_vec.erase(scrollbars_vec.begin()+i); 
			break; //Il n'y a qu'une seule suppression. 
		}
	}
}



void Scrollbar::checkParam()
{
	if(m_len < m_min_len)
		m_len = m_min_len;

	if(m_thickness < m_min_len)
		m_thickness = m_min_thickness;

	int max_pixel_progr = m_len - m_moveable_thing_len;
	if(max_pixel_progr < 0) max_pixel_progr = 0;

	if(m_pixel_progr > max_pixel_progr)
		m_pixel_progr = max_pixel_progr;
	else
	if(m_pixel_progr < 0)
		m_pixel_progr = 0;
}


void Scrollbar::incrProgr(int val)
{
	int max_pixel_progr = m_len - m_moveable_thing_len;
	if(max_pixel_progr < 0) max_pixel_progr = 0;

	int new_progr = m_pixel_progr + val;

	if(new_progr > max_pixel_progr)
		new_progr = max_pixel_progr;
	else
	if(new_progr < 0)
		new_progr = 0;

	m_pixel_progr = new_progr;
}


void Scrollbar::setAreaRects(SDL_Rect& area, SDL_Rect& moveable_thing_area) const
{
	//Définition de la zone globale de la scrollbar, et de l'élément amovible. 
	area.x = m_pos.x;
	area.y = m_pos.y;
	if(m_orient == wi::VERTICAL)
	{
		area.w = m_thickness;
		area.h = m_len;

		moveable_thing_area.x = area.x;
		moveable_thing_area.y = area.y + m_pixel_progr;
		moveable_thing_area.w = area.w;
		moveable_thing_area.h = m_moveable_thing_len;
	}
	else
	{
		area.w = m_len;
		area.h = m_thickness;

		moveable_thing_area.x = area.x + m_pixel_progr;
		moveable_thing_area.y = area.y;
		moveable_thing_area.w = m_moveable_thing_len;
		moveable_thing_area.h = area.h;
	}
}



bool Scrollbar::show(SDL_Renderer* ren)
{
	checkParam();

	static SDL_Rect area;
	static SDL_Rect moveable_thing_area;

	//Définition de la zone globale de la scrollbar, et de l'élément amovible. 
	setAreaRects(area, moveable_thing_area);
	SDL_FRect area_f = frect(area);
	SDL_FRect moveable_thing_area_f = frect(moveable_thing_area);

	bool success=true;
	success & gs::drawRect(ren, &area_f, &m_bg_col, true);
	success & gs::drawRoundedRect(ren, moveable_thing_area_f, 0.35, m_fg_col, true);
	success & gs::drawRect(ren, &area_f, &m_bg_col, false);


	if(not success)
		gs::lastError(std::string("from 'Scrollbar::show()' : ")+gs::lastError());
	
	return success;
}


void Scrollbar::update()
{
	checkParam();

	if(m_to_move_buf != 0)
	{
		incrProgr(m_to_move_buf);
		m_to_move_buf = 0;
		m_moved = true;
	}
	else
		m_moved = false;
}



void Scrollbar::processEvent(const SDL_Event& ev)
{
	static SDL_Rect area;
	static SDL_Rect moveable_thing_area;

	//Définition de la zone globale de la scrollbar, et de l'élément amovible. 
	setAreaRects(area, moveable_thing_area);

	
	if(ev.type == SDL_EVENT_MOUSE_BUTTON_UP and ev.button.button == SDL_BUTTON_LEFT)
		m_mouse_clicked_in = false;
	else
	if(ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN and ev.button.button == SDL_BUTTON_LEFT)
	{
		SDL_Point click_pos={static_cast<int>(ev.button.x), static_cast<int>(ev.button.y)};
		if(SDL_PointInRect(&click_pos, &moveable_thing_area))
			m_mouse_clicked_in = true;
	}
	else
	if(m_mouse_clicked_in)
	{
		if(ev.type == SDL_EVENT_MOUSE_MOTION)
		{
			if(m_orient == wi::VERTICAL)
				m_to_move_buf = ev.motion.yrel;
			else
				m_to_move_buf = ev.motion.xrel;
		}
	}

}


bool Scrollbar::moved() const
{
	return m_moved;
}


unsigned int Scrollbar::setProgr(unsigned int val)
{
	unsigned int prev_progr = m_pixel_progr;
	
	double max_pixel_progr = m_len - m_moveable_thing_len;
	if(max_pixel_progr < 0) max_pixel_progr = 0;

	m_pixel_progr = (unsigned int)(max_pixel_progr*(double(val)/double(m_max_val)));
	checkParam();

	return prev_progr;
}


unsigned int Scrollbar::getProgr()
{
	checkParam();
	
	int max_pixel_progr = m_len - m_moveable_thing_len;
	if(max_pixel_progr < 0) max_pixel_progr = 0;

	double progr_percent = double(m_pixel_progr) / double(max_pixel_progr); 

	unsigned int val = (unsigned int)(progr_percent * double(m_max_val));

	return val; 
}



wi::Scrollbar_Orient Scrollbar::setOrientation(const wi::Scrollbar_Orient& new_orient)
{
	wi::Scrollbar_Orient prev_orient = m_orient;
	m_orient = new_orient;
	return prev_orient;
}

wi::Scrollbar_Orient Scrollbar::getOrientation() const
{
	return m_orient;
}


unsigned int Scrollbar::setLen(unsigned int new_len)
{
	unsigned int prev_len = m_len;
	m_len = new_len;
	m_moveable_thing_len = static_cast<unsigned int>(double(m_len)*0.1);

	return prev_len;
}


unsigned int Scrollbar::getLen() const
{
	return m_len;
}



unsigned int Scrollbar::setThickness(unsigned int new_thickness)
{
	unsigned int prev_thickness = m_thickness;
	m_thickness = new_thickness;
	return prev_thickness;
}

unsigned int Scrollbar::getThickness() const
{
	return m_thickness;
}




Vec2i Scrollbar::setPos(const Vec2i& new_pos)
{
	Vec2i prev_pos = m_pos;
	m_pos = new_pos;
	return prev_pos;
}


Vec2i Scrollbar::getPos() const
{
	return m_pos;
}



unsigned int Scrollbar::setMaxVal(unsigned int max_val)
{
	unsigned int prev_max_v = m_max_val;
	m_max_val = max_val;
	return prev_max_v;
}


unsigned int Scrollbar::getMaxVal() const
{
	return m_max_val;
}



SDL_FColor Scrollbar::setColor(const wi::Scrollbar_Area& area, const SDL_FColor& color)
{
	SDL_FColor prev_col;

	switch(area)
	{
		case wi::MAIN_AREA: prev_col = m_bg_col; m_bg_col = color; break;
		case wi::MOVEABLE_THING_AREA: prev_col = m_fg_col; m_fg_col = color; break;
		default : break;
	}

	return prev_col;
}


SDL_FColor Scrollbar::getColor(const wi::Scrollbar_Area& area) const
{
	switch(area)
	{
		case wi::MAIN_AREA: return m_bg_col; 
		case wi::MOVEABLE_THING_AREA: return m_fg_col;
		default : break;
	}

	return m_bg_col;
}




SDL_Rect Scrollbar::getArea(const wi::Scrollbar_Area& area) const
{
	static SDL_Rect area_rect;
	static SDL_Rect moveable_thing_area_rect;

	//Définition de la zone globale de la scrollbar, et de l'élément amovible. 
	setAreaRects(area_rect, moveable_thing_area_rect);

	switch(area)
	{
		case wi::MAIN_AREA: return area_rect; 
		case wi::MOVEABLE_THING_AREA: return moveable_thing_area_rect;
		default : break;
	}

	return area_rect;
}



} //namespace gs

