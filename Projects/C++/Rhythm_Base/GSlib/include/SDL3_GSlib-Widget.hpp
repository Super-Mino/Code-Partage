#ifndef _SDL3_GSlib_WIDGET_HPP_
#define _SDL3_GSlib_WIDGET_HPP_

#include "SDL3_GSlib.hpp"
#include "SDL3_GSlib-Text.hpp"
#include <unordered_set>

namespace gs
{
	
class Button;
bool initWidgets(gs::Game* g, std::string font_path);
void widgetsProcessEvent(SDL_Event& ev);
void updateWidgets();
bool setButtonText(Button& btn, std::string witch, const char* text, SDL_Color text_color, bool resize_area_w);

	
	

//==============================================================================//
//==============================================================================//
//==============================================================================//



struct Text_Token
{
	SDL_Texture* texture;
	gs::Vec2i size;
};


struct Text_Assembly
{
	Text_Assembly()=default;
	
		gs::Font* font;
		std::unordered_map<std::string, Text_Token> rendered_ones;
		SDL_Color text_color = {255, 255, 255, 255};

	~Text_Assembly();
	
	
	void init(gs::Font* _font);

	bool initTextTextureBuff(SDL_Renderer* ren, SDL_Texture** tex, gs::Vec2i size);

	bool addToken(std::string token);

	int assemble(SDL_Renderer* ren, SDL_Texture* target_texture, std::vector<std::string> tokens);

};








namespace wi
{
	enum Button_State {IDLE, ACTIVE, MOUSE_IN, CLICKED_IN};
	enum Button_Field {AREA, IMG, TEXT};  
	enum Action_Val {DOWN_CLICK=SDL_EVENT_MOUSE_BUTTON_DOWN, UP_CLICK=SDL_EVENT_MOUSE_BUTTON_UP};
	enum Quick_Btns_Field {CHECK_AREA, BUTTONS};
	
}

class Button
{
	public :
		Button();
		~Button();

		bool init(Game* g, SDL_FRect area, const char* img_file_path=nullptr);

		bool loadTexture(Game* g, const char* img_file_path);
		bool setTexture(SDL_Texture* texture);

		bool show(Game* g);

		bool setShow(wi::Button_Field field, bool new_state);

		void setArea(SDL_FRect new_area);

		void incrArea(short x, short y, short w=0, short h=0);

		void setActiveState(bool new_state);

		void incrOffset(wi::Button_Field field, short x, short y);

		void setDestSize(wi::Button_Field field, short w, short h);

		void setColor(wi::Button_State target_state, SDL_FColor c);

		void setTargetEvent(Uint32 target_event);


		void setTargetButtonEvent(Uint8 id);

		bool setCallbackOfTargetEvent(int (*callback)(void*), void* arg);

		friend std::string INTERNAL_USE_switchText(Button& btn, std::string to_witch);

		bool switchText(std::string to_witch="_"); //Avec 'to_witch' != 'active' et de 'idle', ça switch sur le texte qui n'est pas actuellement appliqué. 
		

		bool setSwitchTextByState(bool new_state);

		friend void INTERNAL_USE_editState(Button& btn, wi::Button_State state, bool new_state);
		friend bool setButtonText(Button& btn, std::string witch, const char* text, SDL_Color text_color, bool resize_area_w);
		




		bool isInit() const;

		SDL_Texture* getTexture();

		bool getShow(wi::Button_Field field);
		
		SDL_FRect getArea() const;

		bool getActiveState() const;

		Vec2i getOffset(wi::Button_Field field) const;

		Vec2i getDestSize(wi::Button_Field field) const;

		SDL_FColor getColor(wi::Button_State target_state) const;

		bool mouseIn() const;

		bool clickedIn() const;

		Uint32 getTargetEvent();

		Uint8 getTargetButtonEvent();

		int getLastCallbackReturnVal();

		bool getSwitchTextByState();





	protected:
		bool 			m_is_init=false;
		SDL_FRect 		m_area={0,0,0,0};

		bool 			m_active_state=false;
		
		bool 			m_mouse_in=false;
		bool 			m_clicked_in=false;

		bool  			m_show_text=false;
		bool 			m_show_img=false;
		bool 			m_show_area=true; // Avec une les couleurs ci-dessous selon l'état courant.
		
		Uint32  		m_target_action=wi::DOWN_CLICK;
		Uint8  			m_target_button_id=SDL_BUTTON_LEFT;
		
		int 			(*m_callback)(void*) = nullptr;
		int 			m_last_callback_return_val=0;
		void*  			m_callback_arg=nullptr;


		SDL_Texture* 	m_texture=nullptr;
		Vec2i  			m_texture_offset={0,0};
		Vec2i       	m_texture_dest_size;

		SDL_Texture*  	m_text_texture=nullptr;
		Uint16 			m_button_id=0;
		bool  			m_switch_text_by_state=!false;
		Vec2i  			m_text_offset={0,0};
		Vec2i       	m_text_dest_size;

		SDL_FColor  	m_idle_color={240.f,10.f,10.f,255.f};
		SDL_FColor  	m_active_color={0.f,240.f,0.f,255.f};
		SDL_FColor 	    m_mouse_in_color={100.f,100.f,100.f,50.f};
		SDL_FColor  	m_clicked_in_color={255.f,255.f,40.f,205.f};

};

//==============================================================================//












namespace wi
{
	enum Scrollbar_Orient {VERTICAL, HORIZONTAL};
	enum Scrollbar_Area {MAIN_AREA, MOVEABLE_THING_AREA};
};

class Scrollbar 
{
	public :
		Scrollbar();
		~Scrollbar();

		bool show(SDL_Renderer* ren);
		void update();

		void processEvent(const SDL_Event& ev);

		bool moved() const;
		unsigned int setProgr(unsigned int val);
		unsigned int getProgr();

		wi::Scrollbar_Orient setOrientation(const wi::Scrollbar_Orient& new_orient);
		wi::Scrollbar_Orient getOrientation() const;

		unsigned int setLen(unsigned int new_len);
		unsigned int getLen() const;

		unsigned int setThickness(unsigned int new_thickness);
		unsigned int getThickness() const;

		Vec2i setPos(const Vec2i& new_pos);
		Vec2i getPos() const;

		unsigned int setMaxVal(unsigned int max_val);
		unsigned int getMaxVal() const;

		SDL_FColor setColor(const wi::Scrollbar_Area& area, const SDL_FColor& color);
		SDL_FColor getColor(const wi::Scrollbar_Area& area) const;

		SDL_Rect getArea(const wi::Scrollbar_Area& area=wi::MAIN_AREA) const;



	protected :
	
		void checkParam();
		void incrProgr(int val); //Incrémenter la progression.
		void setAreaRects(SDL_Rect& area, SDL_Rect& moveable_thing_area) const;

		wi::Scrollbar_Orient 	m_orient=wi::VERTICAL; //Orientation.
		bool  					m_mouse_clicked_in=false;
		short 					m_to_move_buf=0; //Valeur qui fait incrémenter la position de l'objet amovible lors de la mise à jour. 
		bool 					m_moved=false; //Témoin du movement à la dernière mise à jour. 

		unsigned int 			m_max_val=100; //Valeur maximale obtenable (peut être changée au besoin).
		unsigned int  			m_pixel_progr=0; //Progression en pixel de l'élément amovible, qui permet de déduire la valeur courante.
		unsigned int 			m_moveable_thing_len=30;
		// unsigned int  			m_no_need_scroll_count=50;

		Vec2i 					m_pos={10, 10};
		unsigned int 			m_len=300; //Longueur de la scrollbar (en pixel).
		unsigned int 			m_thickness=15; //Epaisseur de la scrollbar (en pixel).
		unsigned char 			m_min_len=10; //Longueur minimale.
		unsigned char 			m_min_thickness=4; //Epaisseur minimale.

		SDL_FColor 				m_bg_col={170.f,170.f,170.f,255.f}; //-> de la zone de scroll.
		SDL_FColor 				m_fg_col={240.f,240.f,240.f,255.f}; //-> de l'élément amovible.


};





}//namespace gs

#endif //_SDL2_GSlib_WIDGET_HPP_