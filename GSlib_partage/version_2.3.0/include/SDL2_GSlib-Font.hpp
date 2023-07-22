#ifndef _SDL2_GSlib_FONT_HPP_
#define _SDL2_GSlib_FONT_HPP_
//version modifiée le 14/07/23 (à moins d'oubli de modifier cette ligne).

	#include "SDL2_GSlib.hpp"
	#include <SDL2/SDL_ttf.h>


namespace sm
{


namespace fo
{
	enum Font_Rendering_Type {DEFAULT, SOLID, SHADED, BLENDED};
};

struct INTERNAL_USE_Font_Manger
{
	INTERNAL_USE_Font_Manger() = default;
	~INTERNAL_USE_Font_Manger() 
	{
		if(TTF_WasInit()) 
			TTF_Quit(); 
	}
};

// extern INTERNAL_USE_Font_Manger __FM__;





class Font
{
	public :

		Font() = default;

		
		Font(SDL_Renderer* ren, const char* font_path, size_t ptsize, fo::Font_Rendering_Type default_type = fo::SOLID, bool close_TTF_at_my_exit=false);
		

		~Font(); 

		
		bool init(SDL_Renderer* ren, const char* font_path, size_t ptsize, fo::Font_Rendering_Type default_type = fo::SOLID, bool close_TTF_at_my_exit=false);


		void setShowBackground(bool val);


		bool getShowBackground() const;


		size_t getPtsize() const;

		
		void setRenderingType(fo::Font_Rendering_Type new_type);


		fo::Font_Rendering_Type getRenderingType() const;
		

		void setBackgroundColor(SDL_Color bg_color);
		

		SDL_Color getBackgroundColor() const;
		

		void setRenderer(SDL_Renderer* ren);
		

		SDL_Renderer* getRenderer();
		

		std::string lastError() const;


		SDL_Texture* render(const char* text, SDL_Color text_color, SDL_Rect* text_size_out = nullptr, 
							fo::Font_Rendering_Type rendering_type = fo::DEFAULT, 
							SDL_Color* bg_color = nullptr);
		



	private : 

		fo::Font_Rendering_Type	m_default_type=fo::SOLID;
		size_t 					m_ptsize=32;
		bool 					m_is_init=false;
		bool 					m_close_TTF_at_my_exit=false;
		bool 					m_show_background=true;
		std::string 			m_last_error="no error";
		TTF_Font* 				m_font=nullptr;
		SDL_Color 				m_bg_color={0, 0, 0, 255};
		SDL_Renderer*  			m_renRef=nullptr;

};






}
#endif