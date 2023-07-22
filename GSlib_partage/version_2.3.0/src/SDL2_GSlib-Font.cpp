#include "SDL2_GSlib-Font.hpp"


namespace sm
{

INTERNAL_USE_Font_Manger __FM__;



Font::Font(SDL_Renderer* ren, const char* font_path, size_t ptsize, fo::Font_Rendering_Type default_type, bool close_TTF_at_my_exit)
: m_default_type(default_type), m_ptsize(ptsize), m_close_TTF_at_my_exit(close_TTF_at_my_exit)
{
	init(ren, font_path, ptsize, default_type, close_TTF_at_my_exit);
}

//____________________________________________________________________________________
//____________________________________________________________________________________



Font::~Font() 
{
	TTF_CloseFont(m_font);
	if(m_close_TTF_at_my_exit)
	{
		if(TTF_WasInit())
			TTF_Quit();
	}
}

//____________________________________________________________________________________
//____________________________________________________________________________________


bool Font::init(SDL_Renderer* ren, const char* font_path, size_t ptsize, fo::Font_Rendering_Type default_type, bool close_TTF_at_my_exit)
{
	if(m_is_init) 
	{
		m_last_error = "||Warning|| in 'Font::init()' : cannot init if is already init";
		sm::lastError(m_last_error);
		return false; 
	}

	if(not TTF_WasInit())
		if(TTF_Init() != 0)
		{
			m_last_error = TTF_GetError();
			sm::lastError(sm::str("||Error|| in constructor of 'sm::Font' : cannot init SDL_TTF : ")+ m_last_error);

			m_is_init = false;
			return false;
		}

	
	{
		m_default_type 			= default_type; 
		m_ptsize 				= ptsize;
		m_close_TTF_at_my_exit 	= close_TTF_at_my_exit;
	}


	m_font = TTF_OpenFont(font_path, ptsize);

	if(m_font == nullptr)
	{
		m_last_error = TTF_GetError();
		sm::lastError(m_last_error);

		m_is_init = false;
		return false;
	}
	
	m_renRef = ren;
	m_is_init = true;
	return true;
}


//____________________________________________________________________________________
//____________________________________________________________________________________



void Font::setShowBackground(bool val) {m_show_background = val;}

//____________________________________________________________________________________
//____________________________________________________________________________________


bool Font::getShowBackground() const {return m_show_background;}

//____________________________________________________________________________________
//____________________________________________________________________________________


size_t Font::getPtsize() const {return m_ptsize;}

//____________________________________________________________________________________
//____________________________________________________________________________________


void Font::setRenderingType(fo::Font_Rendering_Type new_type)
{	m_default_type = new_type;	}

//____________________________________________________________________________________
//____________________________________________________________________________________


fo::Font_Rendering_Type Font::getRenderingType() const
{	return m_default_type; 	}

//____________________________________________________________________________________
//____________________________________________________________________________________


void Font::setBackgroundColor(SDL_Color bg_color)
{	m_bg_color = bg_color;	}

//____________________________________________________________________________________
//____________________________________________________________________________________


SDL_Color Font::getBackgroundColor() const
{	return m_bg_color; 		}

//____________________________________________________________________________________
//____________________________________________________________________________________


void Font::setRenderer(SDL_Renderer* ren)
{	m_renRef = ren;			}

//____________________________________________________________________________________
//____________________________________________________________________________________


SDL_Renderer* Font::getRenderer()  
{	return m_renRef;		}

//____________________________________________________________________________________
//____________________________________________________________________________________


std::string Font::lastError() const {return m_last_error;}

//____________________________________________________________________________________
//____________________________________________________________________________________


SDL_Texture* Font::render(const char* text, SDL_Color text_color, SDL_Rect* text_size_out, 
					fo::Font_Rendering_Type rendering_type, 
					SDL_Color* bg_color)
{
	if(not m_is_init)
	{
		m_last_error = "||Warning|| in 'Font::render()', cannot render : not yet init";
		sm::lastError(m_last_error);
	}

	if(m_renRef == nullptr)
	{
		m_last_error = "||Warning|| in 'Font::render()' cannot do without a valid renderer (try to set a valid render)";
		sm::lastError(m_last_error);
		return nullptr;
	}

	fo::Font_Rendering_Type type = rendering_type;

	if(type == fo::DEFAULT)
		type = m_default_type;

	if(bg_color == nullptr)
		bg_color = &m_bg_color;


	SDL_Surface* surface = nullptr;

	switch(type)
	{
		case fo::SOLID: surface = TTF_RenderUTF8_Solid(m_font, text, text_color); break;
		case fo::SHADED: surface = TTF_RenderUTF8_Shaded(m_font, text, text_color, *bg_color); break;
		case fo::BLENDED: surface = TTF_RenderUTF8_Blended(m_font, text, text_color); break;
		case fo::DEFAULT: surface = TTF_RenderUTF8_Solid(m_font, text, text_color); break;
		default : 
			m_last_error = "||Error|| in 'Font::render()' invalid rendering type. It may be an internal error";
			return nullptr;
	}

	if(not m_show_background)
	{
		if(type == fo::SHADED)
		{
			int res = SDL_SetColorKey(surface, SDL_TRUE, ((uint32_t*)surface->pixels)[0]);

			if(res != 0)
			{
				m_last_error = sm::str("||Warning|| in 'Font::render()' cannot hide background, error occured : ") + SDL_GetError();
				sm::lastError(m_last_error);
			}
		}
	}

	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renRef, surface);

	if(textTexture == nullptr)
	{
		m_last_error = std::string("||Error|| in 'Font::render()' : texture failled : ")+ SDL_GetError();
		sm::lastError(m_last_error);
		return nullptr;
	}

	if(text_size_out != nullptr)
	{
		text_size_out->w = surface->w;
		text_size_out->h = surface->h;
	}

	SDL_FreeSurface(surface);

	return textTexture;
}



}


