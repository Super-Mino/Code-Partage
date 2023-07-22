#ifndef _SDL2_GSlib_MAP_HPP_
#define _SDL2_GSlib_MAP_HPP_
//version modifiée le 14/07/23 (à moins d'oubli de modifier cette ligne).

	#include "SDL2_GSlib.hpp"
	#include "SDL2_GSlib-Entity.hpp"




namespace sm /*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*/
{

struct Vec_Int_VecInt
{
	int m_id=0;
	std::vector<int> m_values;
};

struct Tile
{
	short m_value=0;
	bool m_active=true;
	Tile() = default;
	~Tile() {}
};

namespace ma
{

	enum Background_Mode{FILL, PARALLAX};

	enum Parallax_Dest_Appearance{FILL_SCREEN, ORIGINAL_SIZE, CUSTOM_SIZE, FILL_SCREEN_IN_W, FILL_SCREEN_IN_H};



/*

FILL_SCREEN_IN_W et FILL_SCREEN_IN_H
agrandissent ou réduisent l'image en gardant les mêmes proportions.
FILL_SCREEN_IN_W redimensionne de manière à remplir toute la largeur du rendu.
FILL_SCREEN_IN_H redimensionne de manière à remplir toute la longueur du rendu.

*/

	enum Parallax_Param_Field
	{
		APPEARANCE_MODE, 
		CUSTOM_DEST_RECT_X,
		CUSTOM_DEST_RECT_Y,
		CUSTOM_DEST_RECT_W,
		CUSTOM_DEST_RECT_H,
		CURRENT_DEST_RECT_X,
		CURRENT_DEST_RECT_Y,
		CURRENT_DEST_RECT_W,
		CURRENT_DEST_RECT_H,
		BEGIN_POS_X,
		BEGIN_POS_Y,
		CURRENT_POS_X,
		CURRENT_POS_Y,
		CUSTOM_DEST_RECT,
		CURRENT_DEST_RECT,
		BEGIN_POS_XY,
		CURRENT_POS_XY,
		CUSTOM_DEST_RECT_WH,
		CURRENT_DEST_RECT_WH,
	};

};


struct Parallax_Param
{
	ma::Parallax_Dest_Appearance mode=ma::FILL_SCREEN;
	SDL_Rect custom_dest_rect={0, 0, 800, 480};
	SDL_Rect current_dest_rect={0, 0, 800, 480};
	SDL_Point begin_pos={0, 0};
	SDL_Point current_pos={0, 0};

	
	Parallax_Param() = default;
	~Parallax_Param() = default;
};

struct Background
{	
	SDL_Texture* 			m_image=nullptr;
	bool  					m_destroy_ptrs_in_destructor=false;
	ma::Background_Mode 	m_mode=ma::FILL;
	int 					m_w=0, m_h=0;
	bool  					m_is_init=false;
	std::string   			m_1st_image_file_path;

	//pour le mode parallax
	Parallax_Param 			m_param;
	float 					m_start_x=0;
	float 					m_start_y=0;
	float					m_sensibility_x=100.f;
	float  					m_sensibility_y=100.f;
	bool 					m_x_scrolling=true;
	bool  					m_y_scrolling=false;
	bool 					m_move_when_draw=true;	//Active ou désactive l'incrémention de la position lors de l'affichage. Si désactivé, le mouvement doit être fait manuellement par l'utilisateur (si voulu).
	float 					m_x_speed=0.f;	//Vont incrémenter/décrémenter m_start_xy.
	float 					m_y_speed=0.f;



	Background() = default;
	~Background() 
	{
		if(m_destroy_ptrs_in_destructor)
		{
			if(m_image) SDL_DestroyTexture(m_image);
		}
		else
			::sm::addToClearAtTheEnd(m_image);
	}
	bool incrStartX(float val);
	bool incrStartY(float val);

};



struct Map_Layer
{
	/*
	Cette structure est le model de base pour les maps ; il est destiné à être gérér par sm::Map.
	*/

	std::vector<std::vector<Tile>> m_map_array;

	unsigned int 	m_limit_x=0; 			//nombre max de tiles sur les abscisses
	unsigned int 	m_limit_y=0; 			//nombre max de tiles sur les ordonnés
	float 			m_startX=0;				//debut de lecture des pixel virtuels de la map (pixel près, mais pas par rapport à l écran, mais à la map)
	float 			m_startY=0;
	float 			m_sensibility_x=100.f;	//sensibilité d'adaptation aux modifications de startX/Y (e.g: pour 50 % de sensibilité et une incrémentation de 14 au startX, seulement 7 sera ajouté)
	float 			m_sensibility_y=100.f;
	bool 			m_is_init=false;
	bool 			m_is_the_main_layer=false;
	bool 			m_is_visible=true;

	//Les variables qui suivent sont du point de vue de la "camera / nous".
	//C-à-d par exemple que left scrolling c'est quand la "camera" se dirige vers la gauche.
	bool m_up_scrolling_limit_reached=false;
	bool m_down_scrolling_limit_reached=false;
	bool m_left_scrolling_limit_reached=false;
	bool m_right_scrolling_limit_reached=false;

	Map_Layer() = default;
	~Map_Layer() = default;

	bool initTheMapArray();
	void setStartX(float val, short dest_bloc, unsigned short render_w);
	void setStartY(float val, short dest_bloc, unsigned short render_h);
	void incrStartX(float val, short dest_bloc, unsigned short render_w);
	void incrStartY(float val, short dest_bloc, unsigned short render_h);
};




class Map 
{
	public : 


		Map(Game *g, short dest_bloc, std::string title, short src_bloc=32, float zoom=1.f, short number_of_tileset_column=10, short idle_value=-1);

		Map();  

		Map(short dest_bloc, std::string title, short src_bloc=32, float zoom=1.f, short number_of_tileset_column=10, short idle_value=-1);

		Map(Game* g); 

		bool init(Game* g, short dest_bloc, std::string title, short src_bloc=32, float zoom=1.f, short number_of_tileset_column=10, short idle_value=-1);

		~Map(); 
		


		/*-------Globals--------------------------------------------------------------*/
		
		void setBeginX(const float val);

		void setBeginY(const float val);

		void setDestBLOC(const unsigned short val);
		
		void setSrcBLOC(const unsigned short val); 
		
		void setZoom(const float val);

		void incrZoom(const float val);
		
		void setGame(Game* g);		
		
		void setNumberOfTilesetColumn(short val);		

		void incrBeginX(const float val);  		

		void incrBeginY(const float val);		

		void setTileset(const char* pth);  
		
		bool setTileset(SDL_Texture* futur_tileset); 

		void clearTileset(); 

		void setSynchronization(bool new_state);
		//
		//
		int getBeginX() const;		

		int getBeginY() const;
		
		short getDestBLOC() const;

		short getDestBLOCZ() const;	

		short getSrcBLOC() const;
		
		float getZoom() const;
		
		SDL_Texture* getTileset() const;

		bool isTilesetLoaded() const;

		bool isThereAnAffectedGame() const;		

		int getNumberOfTilesetColumn() const;
		
		bool getSynchronization() const;
		
		short getNumberOfLayers() const;
	




		/*-------Layers--------------------------------------------------------------*/
		
		bool addLayer(const char* file_path, const char* layer_str_ID, float sensibility_x=100.f, float sensibility_y=100.f, bool is_the_main_layer=false);
		
		bool drawLayer(unsigned short layer_index); 

		bool drawLayer(); 

		void popBackLayer();
		//
		//
		//
		bool setStartX(unsigned short layer_index, const float val);
			
		bool setStartY(unsigned short layer_index, const float val);
			
		bool incrStartX(unsigned short layer_index, const float val);  	

		bool incrStartY(unsigned short layer_index, const float val);

		bool setStartX(const float val);

		bool setStartY(const float val);

		bool incrStartX(const float val);

		bool incrStartY(const float val);
		
		void setIdleValue(short val, bool restore_previous=true);

		bool setActiveStateOfTilesOf(short tile_val, unsigned short layer_index, bool new_state);

		bool setActiveStateOfTilesOf(short tile_val, bool new_state);

		bool setTileActiveStateAt(unsigned short x, unsigned short y, unsigned short layer_index, bool new_state);
		
		bool setTileValueAt(unsigned short x, unsigned short y, unsigned short layer_index, short new_value);

		bool setSensibilityX(unsigned short layer_index, float new_sensibility);

		bool setSensibilityY(unsigned short layer_index, float new_sensibility);

		bool setShowLayer(unsigned short layer_index, bool new_state);

		bool setShowLayers(bool new_state);
			
		bool setMainLayer(unsigned short layer_index);
		//
		//
		int getStartX(unsigned short layer_index) const;		

		int getStartY(unsigned short layer_index) const;

		float getStartX_f(unsigned short layer_index) const;		

		float getStartY_f(unsigned short layer_index) const;

		int getStartX() const;

		int getStartY() const;

		float getStartX_f() const;

		float getStartY_f() const;

		short getLimitX(unsigned short layer_index) const;		

		short getLimitY(unsigned short layer_index) const;

		short getIdleValue() const; 
		
		bool getTileActiveStateAt(unsigned short x, unsigned short y, unsigned short layer_index) const;

		short getTileValueAt(unsigned short x, unsigned short y, unsigned short layer_index) const;

		float getSensibilityX(unsigned short layer_index);

		float getSensibilityY(unsigned short layer_index);

		bool getShowLayer(unsigned short layer_index);
		
		bool isVisibleLayer(unsigned short layer_index);
		
		bool haveMainLayer() const;
		
		short getMainLayerIndex() const;

		bool isInvalidLayerIndex(unsigned short layer_index) const;


		
		
		
		/*-------Collisions--------------------------------------------------------------*/

		bool setAlsoAsCollisionLayer(unsigned short layer_index, std::vector<int> collision_tiles_value);
		
		bool isCollisionTile(unsigned short x, unsigned short y, unsigned short layer_index);

		bool hitCollisionTile(Body& body, short layer_index =-1);

		bool hitCollisionTile(Entity& entity, short layer_index =-1);

		bool willHitCollisionTile(Body& body, float total_pixel_incr_x, float total_pixel_incr_y, short layer_index =-1, float* dist_x_before_hit__out=nullptr, float* dist_y_before_hit__out=nullptr);

		bool willHitCollisionTile(Entity& entity, float total_pixel_incr_x, float total_pixel_incr_y, short layer_index=-1, float* dist_x_before_hit__out=nullptr, float* dist_y_before_hit__out=nullptr);
		
		bool addCollisionTilesValues(unsigned short layer_index, std::vector<int> values);
		
		bool removeCollisionTilesValues(unsigned short layer_index, std::vector<int> values);
		
		std::vector<int> getCollisionTilesValues(unsigned short layer_index);

		




		/*-------Backgrounds--------------------------------------------------------------*/
		
		bool addBackgroundStack(uint16_t count=1);

		bool popBackBackgroundStack();
		
		bool popBackBackground(short stack_index);

		bool isInvalidBackgroundStackIndex(short stack_index) const;
		
		bool isInvalidBackgroundIndex(short stack_index, short background_index); 
		
		bool addBackground(short stack_index, const char* file_path, ma::Background_Mode mode=ma::FILL, float sensibility_x=100.f, float sensibility_y=100.f, bool x_scrolling=true, bool y_scrolling=true);
			
		bool drawBackground(short stack_index=-1, short background_index=-1);
		
		bool moveBackground(float x_speed, float y_speed, short stack_index=-1, short background_index=-1);
		//
		//
		//
		bool changeBackgroundMode(short stack_index, short background_index, ma::Background_Mode mode);

		bool setBackgroundMoveWhenDraw(short stack_index, short background_index, bool new_state);
		
		bool setBackgroundSpeedX(short stack_index, short background_index, float val);
		
		bool setBackgroundSpeedY(short stack_index, short background_index, float val);

		bool setBackgroundSpeedXY(short stack_index, short background_index, float val_x, float val_y);

		bool editBackgroundParallaxParam(short stack_index, short background_index, ma::Parallax_Param_Field field, int solo_val, SDL_Rect non_solo_vals={0, 0, 100, 100});

		bool setBackgroundSensibilityX(short stack_index, short background_index, float val);
		
		bool setBackgroundSensibilityY(short stack_index, short background_index, float val);
		
		bool setBackgroundSensibilityXY(short stack_index, short background_index, float val_x, float val_y);

		bool setBackgroundScrollingXActiveState(short stack_index, short background_index, bool new_state);
		
		bool setBackgroundScrollingYActiveState(short stack_index, short background_index, bool new_state);
		
		bool setBackgroundScrollingXYActiveState(short stack_index, short background_index, bool new_state_x, bool new_state_y);
		
		bool setBackgroundImage(short stack_index, short background_index, SDL_Texture* texture, bool destroy_previous=false);
		
		bool setBackgroundImage(short stack_index, short background_index, const char* file_path, bool destroy_previous=false);

		
		/* Non signé pour choisir un background précis dans une stack précise. */
		short getNbOfBackgroundStacks() const;
		
		short getNbOfBackgrounds(unsigned short stack_index) const;

		Parallax_Param* getBackgroundParallaxParam(unsigned short stack_index=0, unsigned short background_index=0);

		SDL_Rect getBackgroundParallaxParam(unsigned short stack_index, unsigned short background_index, ma::Parallax_Param_Field field);

		bool getBackgroundMoveWhenDraw(unsigned short stack_index, unsigned short background_index);
		
		float getBackgroundSpeedX(unsigned short stack_index, unsigned short background_index);
		
		float getBackgroundSpeedY(unsigned short stack_index, unsigned short background_index);
		
		float getBackgroundSensibilityX(unsigned short stack_index, unsigned short background_index);
		
		float getBackgroundSensibilityY(unsigned short stack_index, unsigned short background_index);
		
		bool getBackgroundScrollingXActiveState(unsigned short stack_index, unsigned short background_index);
		
		bool getBackgroundScrollingYActiveState(unsigned short stack_index, unsigned short background_index);
		
		SDL_Texture* getBackgroundImage(unsigned short stack_index, unsigned short background_index);	
			




 


	private : 

		bool INTERNAL_USE_stackIndexIsOk(std::string function_name, short stack_index, bool& is_default_index) const;
		bool INTERNAL_USE_backgroundCooAreOk(std::string function_name, short stack_index, bool& is_default_stack_index, short background_index, bool& is_default_background_index);
		std::vector<Background*>* INTERNAL_USE_getTargetBackgrounds(std::string function_name, short stack_index, short background_index);
		std::vector<Background*> m_helper_return_bg_array;


		bool INTERNAL_USE_backgroundBasicVeririf(std::string function_name, short background_index, bool& is_default_index);
		// bool INTERNAL_USE_drawBackOrForeground(Wallpaper_type type, std::string function_name, short index);

		bool INTERNAL_USE_check_tile_access_is_ok(std::string function_name, unsigned short x, unsigned short y, unsigned short layer_index) const;
		bool m_helper__error_at_last_call_of_isCollisionTile=false;
		bool m_helper__error_at_last_call_of_hitCollisionTile=false;
		bool m_destroy_ptrs_in_destructor=false;

		bool m_is_init=false;
		bool m_synchronized=true;

		bool m_have_main_layer=false;
		short m_main_layer_index=-1;

		Game* m_game=nullptr;			

		std::string m_title{"no_titled_map"};

		SDL_Texture* m_tileset=nullptr;			

		std::vector<Map_Layer> m_layers;
		short m_layers_count=0;

		//traitement des backgrounds (avec possibilité d'avoir plusieurs lots de backgrounds)
		std::vector<std::vector<Background>> m_backgrounds_stacks;

		//les infos pour traiter les collisions
		std::vector<Vec_Int_VecInt> m_collision_layers;

		//la valeur de tile pour laquel, à l'initialisation, la tile est noté comme inactive
		short m_idle_value=-1; 

		//debut de lecture des pixel virtuels de la map (pixel près, mais pas par rapport à l écran, 
		// mais à la map)
		double m_startX=0, m_startY=0;//ceux du layer principal	
		
		//à partir d'où sur l'écran (la fenêtre ou la target) on va se référer comme point de départ des calculs de blittage.
		double m_beginX=0, m_beginY=0;

		/*taille pour les blocs de destinations et source (chacun au pixel pres dans son domaine)*/ 
		short m_destBLOC=32, m_srcBLOC=32;

		short m_number_of_tileset_column=10;

		/*zoom de la map qui viendra se multiplier au destBLOC pour l agrandir ou le retrecire*/
		double m_zoom=1.f;

		/*destBLOC et srcBLOC sont par defaut pour des blocs carrés, qui on la même taille 
		des quatre cotés*/ 

};

};/*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*/


/*choses à faire : rajouter l utilisation du dest de background -> peut être déjà fait
				 : rajouter des getW/H pour le tilest et le background 
				 : remplacer ces fonction : SDL_Rect* getBackgroundSrc();  SDL_Rect* gatBackgroundDest();
				 : pourquoi ne pas rajouter une sorte de replaceTileValue() (une sorte de setTileValueAt() pour toutes celles qui ont une certaine valeur donnée). A réflechir.

		

*/

#endif