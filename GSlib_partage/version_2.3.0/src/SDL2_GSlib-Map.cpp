#include "SDL2_GSlib-Map.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <format>

//version modifiée le 14/07/23 (à moins d'oubli de modifier cette ligne)



namespace fs = std::filesystem;

namespace sm
{


using namespace ma;


bool Background::incrStartX(float val)
{
	double coef = m_sensibility_x / 100.f;
	val *= coef;

	m_start_x+= val;
	m_param.current_pos.x = m_param.begin_pos.x + m_start_x;

	
	if(std::abs(m_start_x) >= m_param.current_dest_rect.w) 
		m_start_x = 0;
	
	return true;
}


bool Background::incrStartY(float val)
{
	double coef = m_sensibility_y / 100.f;
	val *= coef;

	m_start_y+= val;
	m_param.current_pos.y = m_param.begin_pos.y + m_start_y;

	
	if(std::abs(m_start_y) >= m_param.current_dest_rect.h) 
		m_start_y = 0;

	return true;
}



/******************************************************************************************/



















bool Map_Layer::initTheMapArray()
{
	if(m_is_init)
	{
		sm::lastError("||Warning|| in 'Map_Layer::initWithTheValues()', already initialized");
		return false;
	}

	m_map_array.clear();
	m_map_array.resize(m_limit_y);

	for(int i=0; i< m_limit_y; i++)
	{
		m_map_array[i].resize(m_limit_x);
	}

	m_is_init = true;
	return m_is_init;
}




void Map_Layer::setStartX(float val, short dest_bloc, unsigned short render_w)
{
	/*on prend le pourcentage de la sensibilité*/
	float coef = m_sensibility_x/100.f;
	val *= coef;
	m_startX = val;

	{
		float max_start = m_limit_x * dest_bloc;
			max_start -= render_w;

		if(m_startX >= max_start)
		{
			m_startX = max_start;
			m_right_scrolling_limit_reached=true;
		}
		else
			m_right_scrolling_limit_reached=false;

		if(m_startX <= 0)
		{
			m_startX = 0;
			m_left_scrolling_limit_reached=true;
		}
		else 
			m_left_scrolling_limit_reached=false;

		if(m_sensibility_x == 0)
		{
			//Afin que ceux qui ne bougent pas quoi qu'il arrive sur l'instant, ne bloquent pas les autres si la synchronisation est activée.
			// m_up_scrolling_limit_reached=false;
			// m_down_scrolling_limit_reached=false;
			m_left_scrolling_limit_reached=false;
			m_right_scrolling_limit_reached=false;
		}

	}

}

void Map_Layer::setStartY(float val, short dest_bloc, unsigned short render_h)
{
	float coef = m_sensibility_y/100.f;
	val *= coef;
	m_startY = val;

	{
		float max_start = m_limit_y * dest_bloc;
			max_start -= render_h;

		if(m_startY >= max_start)
		{
			m_startY = max_start;
			m_down_scrolling_limit_reached=true;
		}
		else
			m_down_scrolling_limit_reached=false;

		if(m_startY <= 0)
		{
			m_startY = 0;
			m_up_scrolling_limit_reached=true;
		}
		else
			m_up_scrolling_limit_reached=false;

		if(m_sensibility_y == 0)
		{
			m_up_scrolling_limit_reached=false;
			m_down_scrolling_limit_reached=false;
			// m_left_scrolling_limit_reached=false;
			// m_right_scrolling_limit_reached=false;
		}
	}
}

void Map_Layer::incrStartX(float val, short dest_bloc, unsigned short render_w)
{
	float coef = m_sensibility_x/100.f;
	val *= coef;
	m_startX += val;

	{
		float max_start = m_limit_x * dest_bloc;
			max_start -= render_w;

		if(m_startX >= max_start)
		{
			m_startX = max_start;
			m_right_scrolling_limit_reached=true;
		}
		else
			m_right_scrolling_limit_reached=false;

		if(m_startX <= 0)
		{
			m_startX = 0;
			m_left_scrolling_limit_reached=true;
		}
		else
			m_left_scrolling_limit_reached=false;

		if(m_sensibility_x == 0)
		{
		// 	m_up_scrolling_limit_reached=false;
		// 	m_down_scrolling_limit_reached=false;
			m_left_scrolling_limit_reached=false;
			m_right_scrolling_limit_reached=false;
		}
	}
}

void Map_Layer::incrStartY(float val, short dest_bloc, unsigned short render_h)
{
	float coef = m_sensibility_y/100.f;
	val *= coef;
	m_startY += val;

	{
		float max_start = m_limit_y * dest_bloc;
			max_start -= render_h;

		if(m_startY >= max_start)
		{
			m_startY = max_start;
			m_down_scrolling_limit_reached=true;
		}
		else
			m_down_scrolling_limit_reached=false;

		if(m_startY <= 0)
		{
			m_startY = 0;
			m_up_scrolling_limit_reached=true;
		}
		else
			m_up_scrolling_limit_reached=false;

		if(m_sensibility_y == 0)
		{
			m_up_scrolling_limit_reached=false;
			m_down_scrolling_limit_reached=false;
			// m_left_scrolling_limit_reached=false;
			// m_right_scrolling_limit_reached=false;
		}
	}
}







/******************************************************************************************/





















Map::Map(Game* g, short dest_bloc, std::string title, short src_bloc, float zoom, short number_of_tileset_column, short idle_value)
: 	m_game(g), m_destBLOC(dest_bloc), m_title(title), m_srcBLOC(src_bloc), m_zoom(zoom), m_number_of_tileset_column(number_of_tileset_column), m_idle_value(idle_value), m_is_init(true)
{/*le plus complet en terme de renseignements*/}		
 


Map::Map()
{/*tout par défaut*/}  



Map::Map(short dest_bloc, std::string title, short src_bloc, float zoom, short number_of_tileset_column, short idle_value)
: Map(nullptr, dest_bloc, title, src_bloc, zoom, number_of_tileset_column, idle_value)
{/*tout sauf la sm::Game* */}



Map::Map(Game* g) 
: m_game(g)
{/*seule la sm::Game* */}



bool Map::init(Game* g, short dest_bloc, std::string title, short src_bloc, float zoom, short number_of_tileset_column, short idle_value)
{
	m_game = g;
	m_destBLOC = dest_bloc;
	m_title = title;
	m_srcBLOC = src_bloc;
	m_zoom = zoom;
	m_number_of_tileset_column = number_of_tileset_column;
	setIdleValue(idle_value, true);
	m_is_init = true;
	return true;
}



Map::~Map()
{
	if(m_destroy_ptrs_in_destructor)
		clearTileset();
	else
		::sm::addToClearAtTheEnd(m_tileset);
};































//________________________________________________________________________________________
//___Globals______________________________________________________________________________

void Map::setBeginX(const float val) {m_beginX = val;}


void Map::setBeginY(const float val) {m_beginY = val;}


void Map::setDestBLOC(const unsigned short val) { m_destBLOC = (val < 1 ? 1 : val); }


void Map::setSrcBLOC(const unsigned short val) { m_srcBLOC = (val < 1 ? 1 : val); } 


void Map::setZoom(const float val) { m_zoom = val; } 


void Map::incrZoom(const float val) { m_zoom += val; }


void Map::setGame(Game* g) { m_game = g; }


void Map::setNumberOfTilesetColumn(short val) { m_number_of_tileset_column = val; }


void Map::incrBeginX(const float val) { m_beginX += val; }  
		

void Map::incrBeginY(const float val) { m_beginY += val; }


void Map::setTileset(const char* pth)   
{
	clearTileset(); 
	if(m_game != nullptr) 
		m_tileset = m_game->loadImg(pth);
	else
	{
		sm::lastError(std::format("||Warning|| in 'setTileset()' of '{}',  missing a valid 'sm::Game'. Make sure you have set a valid 'sm::Game'.", m_title));
		return;
	}

	if(m_tileset == nullptr)
		sm::lastError(std::format("||Warning|| in 'setTileset()' of '{}', tileset load failled : {} ", m_title, sm::lastError()));
}	


bool Map::setTileset(SDL_Texture* futur_tileset)
{
	if(futur_tileset == nullptr)
	{
		sm::lastError(std::format("||Warning|| in 'setTileset()' of '{}', given texture is invalid", m_title));
		return false;
	}

	::sm::addToClearAtTheEnd(m_tileset);

	m_tileset = futur_tileset;

	return true;
}


void Map::clearTileset() 
{
	if(m_tileset != nullptr)
		SDL_DestroyTexture(m_tileset); 
	m_tileset = nullptr;
}

void Map::setSynchronization(bool new_state)
{
	m_synchronized = new_state;
}










int Map::getBeginX() const {return static_cast<unsigned int>(m_beginX);}


int Map::getBeginY() const {return static_cast<unsigned int>(m_beginY);}


short Map::getDestBLOC() const {return m_destBLOC;}


short Map::getDestBLOCZ() const {return static_cast<float>(m_destBLOC)*m_zoom;}


short Map::getSrcBLOC() const {return m_srcBLOC;}


float Map::getZoom() const {return m_zoom;}


SDL_Texture* Map::getTileset() const {return m_tileset;}


bool Map::isTilesetLoaded() const {if(m_tileset == nullptr) return false; return true;}


bool Map::isThereAnAffectedGame() const {if(m_game == nullptr) return false; return true;}


// SDL_Rect* Map::getBackgroundSrc() {return &backgroundSrc_;}


// SDL_Rect* Map::gatBackgroundDest() {return &backgroundDest_;}


int Map::getNumberOfTilesetColumn() const {return m_number_of_tileset_column;}


bool Map::getSynchronization() const {return m_synchronized;}


short Map::getNumberOfLayers() const
{
	if(m_layers.size() != m_layers_count)
	{
		sm::lastError("||Fatal Error|| (found when called 'Map::getNumberOfLayers()'), m_layers.size() != m_layers_count. Alert the dev if you can please");
		return -1;
	}

	return m_layers_count;
}






















//__________________________________________________________________________________________________________
//____Layers________________________________________________________________________________________________

bool Map::addLayer(const char* file_path, const char* layer_str_ID, float sensibility_x, float sensibility_y, bool is_the_main_layer)
{
	std::string error_msg_begin = sm::str("||Error|| in 'Map::addLayer()' of '") + m_title;
	// std::string warning_msg_begin = sm::str("||Warning|| in 'Map::addLayer()' of '") + m_title;

	fs::path path(file_path);

	if(not exists(path))
	{
		sm::lastError(error_msg_begin +sm::str("', invalid file path"));
		return false;
	}


	std::ifstream file(file_path, std::ios::in);

	if(not file.is_open())
	{
		sm::lastError(error_msg_begin +sm::str("', cannot open file"));
		return false;
	}


	bool layer_found = false;
	std::string str_ID = std::string("#") + layer_str_ID;

	while(not file.eof())
	{
		std::string token;

		file >> token;

		if(token == str_ID)
		{
			layer_found = true;
			break;
		}
	}

	if(not layer_found)
	{
		sm::lastError(error_msg_begin +sm::str("', cannot find the layer by the string ID. Make sure ")
					+ sm::str("there is the good string ID in the file or in the source code. ")
					+ sm::str("(In the file, have to have a '#' in the begining of the ID (e.g #MyLayer1). In the source ")
					+ sm::str("code, have to ingore the '#' (e.g map.addLayer(\"path\", \"MyLayer1\") ) )"));
		return false;
	}


	m_layers.push_back(Map_Layer());
	m_layers_count++;
	unsigned short layer_index = m_layers_count - 1;

	
	if(is_the_main_layer)
	{
		if(not m_have_main_layer)
		{
			m_layers[layer_index].m_is_the_main_layer = true;
			m_main_layer_index = layer_index;
			m_have_main_layer = true;

			sensibility_x = 100.f;
			sensibility_y = 100.f;
		}
	}
	


	//lecture des limites
	file >> m_layers[layer_index].m_limit_x;
	file >> m_layers[layer_index].m_limit_y;

	//initialisation du layer
	m_layers[layer_index].initTheMapArray();

	//remplissage avec les valeurs
		//les valeurs invalides seront transformées en celle ci dessous, et la tile sera considérée comme inactive
	const short DEFAULT_VALUE = 0;
	const int limit_y = m_layers[layer_index].m_limit_y;
	const int limit_x = m_layers[layer_index].m_limit_x;

	try
	{
		for(int y=0; y< limit_y; y++)
		{
			for(int x=0; x< limit_x; x++)
			{
				if(file.eof())
				{
					sm::lastError(error_msg_begin +sm::str("', end of file reached without finish collecting datas"));
					m_layers.pop_back();
					return false;
				}

				short val;
				bool active_state=true;

				file >> val;

				if(val < 0)
				{
					val = DEFAULT_VALUE;
					active_state = false;
				}
				else
				if(val == m_idle_value)
					active_state = false;


				Tile tile;
					tile.m_value = val;
					tile.m_active = active_state;

				m_layers[layer_index].m_map_array[y][x] = tile;
			}
		}
	}
	catch(std::exception& ex)
	{
		sm::lastError(sm::str("||Error|| occurred in 'sm::Map::addLayer()' of '") + m_title +sm::str("', when read datas ; ")
					+ sm::str(ex.what()));
		m_layers.pop_back();
		return false;
	}
	

	if(sensibility_x < 0)
		sensibility_x = 0;
	if(sensibility_y < 0)
		sensibility_y = 0;

	m_layers[layer_index].m_sensibility_x = sensibility_x;
	m_layers[layer_index].m_sensibility_y = sensibility_y;


	return true;
}



bool Map::drawLayer(unsigned short layer_index) 
{
	std::string error_msg_begin = sm::str("||Error|| in 'sm::Map::drawLayer()' of '") + m_title;
	std::string warning_msg_begin = sm::str("||Warning|| in 'sm::Map::drawLayer()' of '") + m_title;

	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(error_msg_begin + sm::str("', given layer index doesn't exist"));		
		return false;
	}

	if(not m_layers.at(layer_index).m_is_visible)
		return true;

	if(m_game == nullptr)
	{
		sm::lastError(warning_msg_begin + sm::str("', cannot draw : missing a valid 'sm::Game'"));
		return false;
	}

	//prise en compte du zoom, qui baisse/augmente la taille du bloc de destination
	int destBLOC_z = static_cast<float>(m_destBLOC) * m_zoom; 

	//d'où l'on commence à lire (sur les cases du tableau) 
	int mapX = getStartX(layer_index) / destBLOC_z;
	int mapY = getStartY(layer_index) / destBLOC_z;
	

	//à partir d'où (au pixel près) on va commencer à blitter 
	int x1 = getBeginX() -  getStartX(layer_index) % destBLOC_z ;
	int y1 = getBeginY() -  getStartY(layer_index) % destBLOC_z ;

	//nombre maximal de tiles sur chaque axe
	int limit_x = m_layers[layer_index].m_limit_x;
	int limit_y = m_layers[layer_index].m_limit_y;

	//à quelle pixel de l'écran on arrête de blitter:
	//pour commencer on considère qu'on s'arrête au tout dernier pixel, même si est hors écran,
	//puis on apporte une correction si nécessaire.
	int x2 = x1 + limit_x * destBLOC_z;
	int y2 = y1 + limit_y * destBLOC_z;
		
		//si x2 < largeur du rendu : tout va bien, sinon on l'arrête à la bordure
		int x2_max = m_game->getRenW(); 
		if(x2 > x2_max)
			x2 = x2_max;

		int y2_max = m_game->getRenH(); 
		if(y2 > y2_max)
			y2 = y2_max;
	

	//protections contre le crache
	if(mapX <= 0)mapX = 0;
	if(mapY <= 0)mapY = 0;

	
	for(int y = y1; y < y2; y+= destBLOC_z)
	{
		for(int x = x1; x< x2; x+= destBLOC_z)
		{

			if(mapX >= limit_x || mapY >= limit_y)
			{
				sm::lastError(error_msg_begin+ sm::str("', reached out of limits when drawing layer ")
							+ std::to_string(layer_index));
				return false;
			}

			if(m_layers[layer_index].m_map_array[mapY][mapX].m_active)
			{
				int tile_numero = m_layers[layer_index].m_map_array[mapY][mapX].m_value;
				

				int notc = m_number_of_tileset_column;
				int xsrc = (tile_numero % notc) * m_srcBLOC;
				int ysrc = (tile_numero / notc) * m_srcBLOC;

				
				m_game->drawTile(m_tileset, x, y, destBLOC_z, xsrc, ysrc, m_srcBLOC, false); //false pour dessiner sur la target courrante et pas forcément que sur le render de base.
			}

			mapX ++;
		}

		//remise du "curseur" des x au début 
		mapX = getStartX(layer_index) / destBLOC_z;
		mapY ++;
	}


	return true;
}
	


bool Map::drawLayer() 
{
	bool success = true;

	for(int index=0; index< m_layers_count; index++)
	{
		success = (success & drawLayer(index));
	}

	return success;
}



void Map::popBackLayer()
{
	short last_index = m_layers_count - 1;

	if(isInvalidLayerIndex(last_index))
		return;

	if(m_layers[last_index].m_is_the_main_layer)
	{
		m_have_main_layer = false;
		m_main_layer_index = -1; // = non défini
	}

	for(size_t i=0; i< m_collision_layers.size(); i++)
	{
		Vec_Int_VecInt& collision_lay = m_collision_layers.at(i);

		if(collision_lay.m_id == last_index)
		{
			m_collision_layers.erase(m_collision_layers.begin()+i);
			break;
		}
	}

	m_layers.pop_back();

	m_layers_count --; 
}







































bool Map::setStartX(unsigned short layer_index, const float val)   
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::setStartX()', given layer index doesn't exist");
		return false;
	}

	if(m_game == nullptr)
	{
		sm::lastError("||Warning|| in 'Map::setStartX()', invalid sm::Game* ");
		return false;
	}

	m_layers[layer_index].setStartX(val, getDestBLOCZ(), m_game->getRenW());
	return true;
}





bool Map::setStartY(unsigned short layer_index, const float val)   
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::setStartY()', given layer index doesn't exist");
		return false;
	}

	if(m_game == nullptr)
	{
		sm::lastError("||Warning|| in 'Map::setStartY()', invalid sm::Game* ");
		return false;
	}

	m_layers[layer_index].setStartY(val, getDestBLOCZ(), m_game->getRenH());
	return true;
}




bool Map::incrStartX(unsigned short layer_index, const float val) 
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::incrStartX()', given layer index doesn't exist");
		return false;
	}

	if(m_game == nullptr)
	{
		sm::lastError("||Warning|| in 'Map::incrStartX()', invalid sm::Game* ");
		return false;
	} 
	
	m_layers[layer_index].incrStartX(val, getDestBLOCZ(), m_game->getRenW());

	return true;
}  
		

bool Map::incrStartY(unsigned short layer_index, const float val) 
{ 
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::incrStartY()', given layer index doesn't exist");
		return false;
	}

	if(m_game == nullptr)
	{
		sm::lastError("||Warning|| in 'Map::incrStartY()', invalid sm::Game* ");
		return false;
	} 
	
	m_layers[layer_index].incrStartY(val, getDestBLOCZ(), m_game->getRenH());

	return true;
}


		

bool Map::setStartX(const float val)
{
	//Pour cette fonction qui est un setter sur tous, la synchronisation des 'incr' ne rentre pas en compte
	bool success = true;
	for(int index = 0; index < m_layers_count; index++)
	{
		success = (success & setStartX(index, val));
	}

	return success;
}



bool Map::setStartY(const float val)
{
	bool success = true;
	for(int index = 0; index < m_layers_count; index++)
	{
		success = (success & setStartY(index, val));
	}

	return success;
}





bool Map::incrStartX(const float val)
{
	/*En mode synchronisé, les layers ne peuvent bouger que si eux tous peuvent bouger (dans tel ou tel sens)*/
	
	bool success = true;

	//calculs préparatoires pour savoir si tout le monde peut scroller
	if(m_synchronized && val != 0)
	{
		bool there_is_one_who_cannot = false;
		bool go_to_left = (val < 0 ? true : false);

		if(go_to_left)
		{
			for(const Map_Layer& layer : m_layers)
			{
				if(layer.m_left_scrolling_limit_reached)
				{
					there_is_one_who_cannot = true;
					break;
				}
			}
		}
		else // = go to right
		{
			for(const Map_Layer& layer : m_layers)
			{
				if(layer.m_right_scrolling_limit_reached)
				{
					there_is_one_who_cannot = true;
					break;
				}
			}
		}

		if(there_is_one_who_cannot)
			return true;
	}


	for(int index = 0; index < m_layers_count; index++)
	{
		success = (success & incrStartX(index, val));
	}

	return success;
}  
	




bool Map::incrStartY(const float val)
{
	bool success = true;

	if(m_synchronized && val != 0)
	{
		bool there_is_one_who_cannot = false;
		bool go_to_up = (val < 0 ? true : false);

		if(go_to_up)
		{
			for(const Map_Layer& layer : m_layers)
			{
				if(layer.m_up_scrolling_limit_reached)
				{
					there_is_one_who_cannot = true;
					break;
				}
			}
		}
		else // = go to down
		{
			for(const Map_Layer& layer : m_layers)
			{
				if(layer.m_down_scrolling_limit_reached)
				{
					there_is_one_who_cannot = true;
					break;
				}
			}
		}

		if(there_is_one_who_cannot)
			return true;
	}


	for(int index = 0; index < m_layers_count; index++)
	{
		success = (success & incrStartY(index, val));
	}

	return success;
}



void Map::setIdleValue(short val, bool restore_previous)
{
	/*
	Cette fonction attribue une valeur de tuile (pour tous les layers) qui sera considérée comme inactif. 
	C-à-d que toute les tuiles ayant comme valeur cette valeur seront inactives (ignorées lors de l'affichage ou/et des collisions).
	`restore_previous' permet de si jamais on avait utilisé cette fonction précédemment, de ne pas garder les tuiles 
	concernées par l'encienne valeur de `m_idle_value' incatives, alors qu'elles ne sont potentiellement plus concernées par la 
	nouvelle valeur. Par ailleurs l'utilisateur peut interdire cette restauration en mentionnant un 'false' ; 'true' est par défaut. 
	*/

	if(restore_previous)
	{
		for(int i=0; i< m_layers_count; ++i)
		{
			setActiveStateOfTilesOf(m_idle_value, i, true);
		}
	}

	m_idle_value = val;

	for(int i=0; i< m_layers_count; ++i)
	{
		setActiveStateOfTilesOf(m_idle_value, i, false);
	}
}


bool Map::setActiveStateOfTilesOf(short tile_val, unsigned short layer_index, bool new_state)
{
	/*
	Pour un layer donné, cette fonction attribue un état actif ou inactif donné par `new_state', à toutes les tuiles ayant la même valeur que `tile_val'. 
	*/
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::setActiveStateOfTilesOf()', given layer index doesn't exist. ");
		return false;
	}

	if(not m_layers.at(layer_index).m_is_init)
		return false;

	short limit_x = m_layers[layer_index].m_limit_x;
	short limit_y = m_layers[layer_index].m_limit_y;

	for(int i=0; i< limit_y; i++)
	{
		for(int j=0; j< limit_x; j++)
		{
			Tile& tile = m_layers[layer_index].m_map_array[i][j];

			if(tile.m_value == tile_val)
				tile.m_active = new_state;
		}
	}

	return true;
}


bool Map::setActiveStateOfTilesOf(short tile_val, bool new_state)
{
	bool success = true;
	for(int i=0; i< m_layers_count; i++)
	{
		success = (success & setActiveStateOfTilesOf(tile_val, i, new_state));
	}

	return success;
}



bool Map::setTileActiveStateAt(unsigned short x, unsigned short y, unsigned short layer_index, bool new_state)
{
	if(not INTERNAL_USE_check_tile_access_is_ok(sm::str("`Map::setTileActiveStateAt()'"), x, y, layer_index))
		return false;

	m_layers[layer_index].m_map_array[y][x].m_active = new_state;

	return true;
}



bool Map::setTileValueAt(unsigned short x, unsigned short y, unsigned short layer_index, short new_value)
{
	if(not INTERNAL_USE_check_tile_access_is_ok(sm::str("`Map::setTileValueAt()'"), x, y, layer_index))
		return false;

	if(new_value < 0)
	{
		sm::lastError(sm::str("||Warning|| in 'Map::setTileValueAt()' of '")+m_title+sm::str("', tile value cannot be negative. [given value : ")+std::to_string(new_value)+sm::str("]. "));
		return false;
	}

	m_layers[layer_index].m_map_array[y][x].m_value = new_value;

	return true;
}


bool Map::setSensibilityX(unsigned short layer_index, float new_sensibility)
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::setSensibilityX()', given layer index doesn't exist. ");
		return false;
	}

	if(m_layers[layer_index].m_is_the_main_layer)
	{
		sm::lastError("||Warning|| in 'Map::setSensibilityX()', connot change the sensibility of the main layer. ");
		return false;
	}

	if(new_sensibility < 0)
		new_sensibility = 0;

	m_layers[layer_index].m_sensibility_x = new_sensibility;

	return true;
}


bool Map::setSensibilityY(unsigned short layer_index, float new_sensibility)
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::setSensibilityY()', given layer index doesn't exist. ");
		return false;
	}

	if(m_layers[layer_index].m_is_the_main_layer)
	{
		sm::lastError("||Warning|| in 'Map::setSensibilityY()', connot change the sensibility of the main layer. ");
		return false;
	}

	if(new_sensibility < 0)
		new_sensibility = 0;

	m_layers[layer_index].m_sensibility_y = new_sensibility;

	return true;
}





bool Map::setShowLayer(unsigned short layer_index, bool new_state)
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Warning|| in 'sm::Map::setShowLayer()' of '")+m_title+sm::str("', given layer index doesn't exist. [given index : ")+std::to_string(layer_index)+sm::str("] "));
		return false;
	}

	m_layers.at(layer_index).m_is_visible = new_state;

	return true;
}




bool Map::setShowLayers(bool new_state)
{
	if(m_layers.empty())
	{
		sm::lastError(sm::str("||Warning|| in 'sm::Map::setShowLayers()' of '")+m_title+sm::str("', no have any layer in this 'sm::Map'. "));
		return false;
	}

	bool success = true;
	for(int i=0; i< m_layers.size(); i++)
	{
		success = (success & setShowLayer(i, new_state));
	}

	if(not success)
		sm::lastError(sm::str("from 'sm::Map::setShowLayers' : ")+ sm::lastError());

	return success;
}





bool Map::setMainLayer(unsigned short layer_index)
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Warning|| in 'Map::setMainLayer()' of '") +m_title
					+ sm::str("', given layer index doesn't exist. "));
		return false;
	}

	if(m_have_main_layer)
	{
		m_layers[m_main_layer_index].m_is_the_main_layer = false;
	}

	m_layers[layer_index].m_is_the_main_layer = true;
	m_layers[layer_index].m_sensibility_x = 100.f;
	m_layers[layer_index].m_sensibility_y = 100.f;
	m_main_layer_index = layer_index;

	return true;
}














int Map::getStartX(unsigned short layer_index) const 
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::getStartX()', given layer index doesn't exist");
		return -1;
	}

	float start_x = m_layers[layer_index].m_startX;
	return static_cast<unsigned int>(start_x);
}


int Map::getStartY(unsigned short layer_index) const 
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::getStartY()', given layer index doesn't exist");
		return -1;
	}

	float start_y = m_layers[layer_index].m_startY;
	return static_cast<unsigned int>(start_y);
}


float Map::getStartX_f(unsigned short layer_index) const 
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::getStartX_f()', given layer index doesn't exist");
		return -1.f;
	}

	return m_layers[layer_index].m_startX;
}
		

float Map::getStartY_f(unsigned short layer_index) const 
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::getStartY_f()', given layer index doesn't exist");
		return -1.f;
	}

	return m_layers[layer_index].m_startY;
}


int Map::getStartX() const
{
	if(m_have_main_layer)
		return getStartX(m_main_layer_index);
	return getStartX(0);
} 

int Map::getStartY() const
{
	if(m_have_main_layer)
		return getStartY(m_main_layer_index);
	return getStartY(0);
} 

float Map::getStartX_f() const
{
	if(m_have_main_layer)
		return getStartX_f(m_main_layer_index);
	return getStartX_f(0);
} 

float Map::getStartY_f() const
{
	if(m_have_main_layer)
		return getStartY_f(m_main_layer_index);
	return getStartY_f(0);
} 


short Map::getLimitX(unsigned short layer_index) const 
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::getLimitX()', given layer index doesn't exist");
		return -1;
	}

	return m_layers[layer_index].m_limit_x;
}


short Map::getLimitY(unsigned short layer_index) const 
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::getLimitY()', given layer index doesn't exist");
		return -1;
	}

	return m_layers[layer_index].m_limit_y;
}


short Map::getIdleValue() const 
{
	return m_idle_value;
}


bool Map::getTileActiveStateAt(unsigned short x, unsigned short y, unsigned short layer_index) const
{
	
	if(not INTERNAL_USE_check_tile_access_is_ok(sm::str("`Map::getTileActiveStateAt()'"), x, y, layer_index))
		return false;

	return m_layers[layer_index].m_map_array[y][x].m_active;
}


short Map::getTileValueAt(unsigned short x, unsigned short y, unsigned short layer_index) const
{
	if(not INTERNAL_USE_check_tile_access_is_ok(sm::str("`Map::getTileValueAt()'"), x, y, layer_index))
		return -1;

	return m_layers[layer_index].m_map_array[y][x].m_value;
}






float Map::getSensibilityX(unsigned short layer_index)
{	
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::getSensibilityX()', given layer index doesn't exist");
		return -1;
	}

	return m_layers[layer_index].m_sensibility_x;
}


float Map::getSensibilityY(unsigned short layer_index)
{	
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError("||Warning|| in 'Map::getSensibilityY()', given layer index doesn't exist");
		return -1;
	}

	return m_layers[layer_index].m_sensibility_y;
}



bool Map::getShowLayer(unsigned short layer_index)
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Warning|| in 'sm::Map::getShowLayer()' of '")+m_title+sm::str("', given layer index doesn't exist. [given index : ")+std::to_string(layer_index)+sm::str("] "));
		return false;
	}

	return m_layers.at(layer_index).m_is_visible;
}



bool Map::isVisibleLayer(unsigned short layer_index)
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Warning|| in 'sm::Map::isVisibleLayer()' of '")+m_title+sm::str("', given layer index doesn't exist. [given index : ")+std::to_string(layer_index)+sm::str("] "));
		return false;
	}

	return m_layers.at(layer_index).m_is_visible;
}



bool Map::haveMainLayer() const
{
	return m_have_main_layer;
}


short Map::getMainLayerIndex() const
{
	return m_main_layer_index;
}



bool Map::isInvalidLayerIndex(unsigned short layer_index) const
{
	short max_layer_index = m_layers_count -1;
	if(layer_index > max_layer_index)
		return true;
	return false;
}



bool Map::INTERNAL_USE_check_tile_access_is_ok(std::string function_name, unsigned short x, unsigned short y, unsigned short layer_index) const
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Error|| in function ")+function_name+sm::str(", given layer index doesn't exist. [given layer : ")+std::to_string(layer_index)+sm::str("]. "));
		return false;
	}

	if(not m_layers[layer_index].m_is_init)
	{
		sm::lastError(sm::str("||Error|| in function ")+function_name+sm::str(", this layer seems not yet init. [given layer : ")+std::to_string(layer_index)+sm::str("]. "));
		return false;
	}

	const int limit_x = m_layers[layer_index].m_limit_x;
	const int limit_y = m_layers[layer_index].m_limit_y;

	if(x >= limit_x)
	{
		lastError(sm::str("||Error|| in function ")+function_name+sm::str(", given parameter `x' is invalid for layer ")+std::to_string(layer_index)+sm::str(". [given x : ")+std::to_string(x)+sm::str("]. "));
		return false;
	}
	if(y >= limit_y)
	{
		lastError(sm::str("||Error|| in function ")+function_name+sm::str(", given parameter `y' is invalid for layer ")+std::to_string(layer_index)+sm::str(". [given y : ")+std::to_string(y)+sm::str("]. "));
		return false;
	}

	return true;
}


































//__________________________________________________________________________________________________________
//_____Collisions___________________________________________________________________________________________

bool Map::setAlsoAsCollisionLayer(unsigned short layer_index, std::vector<int> collision_tiles_value)
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Warning|| in 'sm::Map::setAlsoAsCollisionLayer()' of '")+m_title+sm::str("', given layer index doesn't exist. "));
		return false;
	}

	for(auto& collision_lay : m_collision_layers)
	{
		if(collision_lay.m_id == layer_index)
		{
			sm::lastError(sm::str("||Warning|| in 'sm::Map::setAlsoAsCollisionLayer()' of '")+m_title+sm::str("', layer ")+std::to_string(layer_index)+sm::str(" is already set as a collision layer. "));
			return false;
		}
	}

	if(collision_tiles_value.empty())
	{
		sm::lastError(sm::str("||Warning|| in 'sm::Map::setAlsoAsCollisionLayer()' of '")+m_title+sm::str("', cannot add a collision layer without collision tile values. Given 'collision_tiles_value' is empty. "));
		return false;
	}

	//afin d'éviter les doubles valeurs
	{
		std::vector<int> filtered_values{collision_tiles_value.front()};

		for(int& val : collision_tiles_value)
		{
			std::vector<int>::iterator it;
			it = std::find(filtered_values.begin(), filtered_values.end(), val);

			bool already_in = (it == filtered_values.end() ? false : true);

			if(not already_in)
				filtered_values.push_back(val);
		}

		collision_tiles_value.swap(filtered_values);
	}

	Vec_Int_VecInt layer_info; 
	layer_info.m_id = layer_index;
	layer_info.m_values = collision_tiles_value;
	m_collision_layers.push_back(layer_info);

	return true;
}







bool Map::isCollisionTile(unsigned short x, unsigned short y, unsigned short layer_index) 
{
	m_helper__error_at_last_call_of_isCollisionTile=false;

	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Warning|| in 'sm::Map::isCollisionTile()' of '")+m_title+sm::str("', given layer index doesn't exist. "));
		m_helper__error_at_last_call_of_isCollisionTile=true;
		return false;
	}


	bool found = false;
	Vec_Int_VecInt* collision_lay_ptr=nullptr;

	for(Vec_Int_VecInt& collision_lay : m_collision_layers)
	{
		if(collision_lay.m_id == layer_index)
		{
			collision_lay_ptr = &collision_lay;
			found = true;
		}
	}

	if(not found or collision_lay_ptr == nullptr)
	{
		sm::lastError(sm::str("||Warning|| in 'sm::Map::isCollisionTile()' of '")+m_title+sm::str("', layer ")+std::to_string(layer_index)+sm::str(" seems not set as a collision layer. "));
		m_helper__error_at_last_call_of_isCollisionTile=true;
		return false;
	}

	short tile_value_at_xy = getTileValueAt(x, y, layer_index);

	if(tile_value_at_xy == -1)
	{
		sm::lastError(sm::str("from 'sm::Map::isCollisionTile()' : ")+sm::lastError());
		m_helper__error_at_last_call_of_isCollisionTile=true;
		return false;
	}

	if(getTileActiveStateAt(x, y, layer_index) == false)
		return false;

	for(const int& collision_tile_val : collision_lay_ptr->m_values)
	{
		if(tile_value_at_xy == collision_tile_val)
			return true;
	}

	return false;
}










bool Map::hitCollisionTile(Body& body, short layer_index) // layer_index = -1 par défaut pour signifier de prendre le premier layer de collision (si présent)
{
	m_helper__error_at_last_call_of_hitCollisionTile = false;


	if(std::empty(m_collision_layers))
	{
		sm::lastError(sm::str("||Warning|| in 'sm::Map::hitCollisionTile()' of '")+m_title+sm::str("', no have any collision layer in this 'sm::Map'."));
		m_helper__error_at_last_call_of_hitCollisionTile = true;
		return false;
	}
	else
	if(layer_index == -1)
	{
		try
		{
			layer_index = m_collision_layers.at(0).m_id;
		}
		catch(std::out_of_range& _er) //normalement avec la vérification ci-dessus, ce bloc catch ne devrait jamais être exécuté
		{
			sm::lastError(std::format("||Error|| Exception occurred in 'sm::Map::hitCollisionTile()' of '{}' about indexing the collision layer. It may be an internal error. The exception : {}", m_title, _er.what()));
			return false;
		}
	}
	else
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Warning|| in 'sm::Map::hitCollisionTile()' of '")+m_title+sm::str("', given layer index doesn't exist. "));
		m_helper__error_at_last_call_of_hitCollisionTile = true;
		return false;
	}
	else
	{
		bool found = false;
		for(Vec_Int_VecInt& collision_lay : m_collision_layers)
		{
			if(layer_index == collision_lay.m_id)
			{
				found = true;
				break;
			}
		}

		if(not found)
		{
			sm::lastError(sm::str("||Warning|| in 'sm::Map::hitCollisionTile()' of '")+m_title+sm::str("', given layer index doesn't exist as a collision layer. "));
			m_helper__error_at_last_call_of_hitCollisionTile = true;
			return false;
		}
	}


	//Pour se faire, premièrement on va calculer la position de chacun de ses 4 points dans la tile-map et voir si au moins un est dans une tile de collision. (pratique de vérifier seulement les quatres coins dans le cas des hitbox plus petits qu'une tuile)
	
	//La position dans la tile-map est : 'position à l'écran (au pixel près, sur la cible de rendu)' + 'décalage de la "caméra" (startX ou Y)'.
	
	/*Petit mémo : 
		pos_à_écran (en pixel) 			= pos_dans_map (en pixel) - startX ou Y (en pixel)
		pos_dans_map 					= pos_à_écran + startX ou Y
		case_dans_map ou tuile_dans_map = pos_dans_map / bloc (taille d'une tuile)

		en ce qui concerne startX ou Y : dépend de la taille du bloc : plus le bloc est petit plus il impacte dans l'avancée dans la map (notamment au niveau des cases/tuiles);
										plus le bloc est grand, moins il impacte.
										Dépend de la taille du bloc et son impact varie, car son unité ne change pas (reste au pixel) tandis que la taille du bloc peut changer.
										On pourrait utiliser un startXY non plus au pixel, mais en pourcentage, ainsi il restera constant peu importe la taille du bloc.
										À voir si j'opterai pour cette solution/optimisation à l'avenir une fois que je serai carré là-dessus.

	*/

	// g = gauche, d = droit, h = haut, b = bas, hg = point haut gauche :)
	SDL_Point hg, hd, bg, bd; 

	hg.x = body.getX() + getStartX(layer_index);
	hg.y = body.getY() + getStartY(layer_index);

	hd.x = hg.x + body.getW()-1;
	hd.y = hg.y; 

	bg.x = hg.x; 
	bg.y = hg.y + body.getH()-1; 

	bd.x = hg.x + body.getW()-1;
	bd.y = hg.y + body.getH()-1; 

	std::vector<SDL_Point> points{hg, hd, bg, bd};
	bool have_one_who_hit = false;

	int limit_x = m_layers.at(layer_index).m_limit_x;
	int limit_y = m_layers.at(layer_index).m_limit_y;
	
	for(const SDL_Point& pt : points)
	{
		short px = pt.x/getDestBLOCZ();
		short py = pt.y/getDestBLOCZ();
		
		if(px < 0 or py < 0 or limit_x <= px or limit_y <= py)
				continue;	//Il n'y a aucune collision à l'extérieur de la map, donc on ignore le point.

		if(isCollisionTile(px, py, layer_index)) 	// /getDestBLOCZ() = conversion de pos_dans_map à tuile_dans_map
		{
			have_one_who_hit = true;
			break;
		}
	}


	//Deuxième partie potentielle : si pas de collision encore détécté et qu'un côté est plus grand que le bloc de destination on vérifie aussi les tuiles entre les deux points des côtés.
	//Note : le bloc de destination est la taille en pixel d'une tuile sur la cible de rendu. getDestBLOC() donne la valeur initiale, getDestBLOCZ() donne la valeur réelle à traiter (où l'influence du zoom est incluse).
	
	if(not have_one_who_hit)
	{
		std::vector<SDL_Point> other_points;

		if( std::abs(hg.y - bg.y) > getDestBLOCZ() ) // = côté gauche et droit de l'hitblox sont plus grands que getDestBLOCZ()
		{
			//ici les points sont directement traité en case
			//Note : j'utilise 'case' et 'tuile' sans distinction :)
			SDL_Point min_side_tile_pos = {hg.x/getDestBLOCZ(), hg.y/getDestBLOCZ()};
			SDL_Point max_side_tile_pos = {bg.x/getDestBLOCZ(), bg.y/getDestBLOCZ()};

			for(int y = min_side_tile_pos.y+1; y< max_side_tile_pos.y; y++) // = collecte des y des tuiles entre les deux points. 'min_side_tile_pos.y' est déjà vérifié plus haut, c'est pour ça que l'on commence avec le potentiel suivant. De même pour 'max_side_tile_pos.y', on ne va pas jusqu'à lui car déjà traité.
			{
				//point sur le côté gauche
				SDL_Point intern_ptl={min_side_tile_pos.x, y};
				//point sur le côté droit
				SDL_Point intern_ptr={hd.x/getDestBLOCZ(), y};

				other_points.push_back(intern_ptl);
				other_points.push_back(intern_ptr);
			}
		}

		if( std::abs(hg.x - hd.x) > getDestBLOCZ() ) // = côté du haut et du bas de l'hitbox sont plus grands que le getDestBLOCZ()
		{
			SDL_Point min_side_tile_pos = {hg.x/getDestBLOCZ(), hg.y/getDestBLOCZ()};
			SDL_Point max_side_tile_pos = {hd.x/getDestBLOCZ(), hd.y/getDestBLOCZ()};

			for(int x = min_side_tile_pos.x+1; x< max_side_tile_pos.x; x++)
			{
				//point sur le côté haut
				SDL_Point intern_ptu = {x, min_side_tile_pos.y};
				//point sur le côté bas
				SDL_Point intern_ptd = {x, bg.y/getDestBLOCZ()};

				other_points.push_back(intern_ptu);
				other_points.push_back(intern_ptd);
			}
		}

		for(const auto& pt : other_points)
		{
			if(pt.x < 0 or pt.y < 0 or limit_x <= pt.x or limit_y <= pt.y)
				continue;	//Il n'y a aucune collision à l'extérieur de la map, donc on ignore le point.
			
			if(isCollisionTile(pt.x, pt.y, layer_index))
			{
				have_one_who_hit = true;
				break;
			}
		}
	}




	//Troisième cas potentiel : parcour des points interieurs.
	if(not have_one_who_hit)
	if(std::abs(hg.y - bg.y) > getDestBLOCZ() or std::abs(hg.x - hd.x) > getDestBLOCZ())
	{
		std::vector<SDL_Point> other_points;

		SDL_Point top_left = {hg.x/getDestBLOCZ(), hg.y/getDestBLOCZ()}; 	//la première tuile
		SDL_Point low_right = {bd.x/getDestBLOCZ(), bd.y/getDestBLOCZ()};	//la dernière tuile

		//ici seuls les tuiles de l'interieur sont vétifiés (aucune tuiles ici ne touche un bord de l'hitbox puisque ces points sont déjà vérifiés plus haut)
		for(int y=top_left.y+1; y< low_right.y; y++)
		{
			for(int x=top_left.x+1; x< low_right.x; x++)
			{
				SDL_Point pt = {x, y};
				other_points.push_back(pt);
			}
		}

		for(const auto& pt : other_points)
		{
			if(pt.x < 0 or pt.y < 0 or limit_x <= pt.x or limit_y <= pt.y)
				continue;	//Il n'y a aucune collision à l'extérieur de la map, donc on ignore le point.
			
			if(isCollisionTile(pt.x, pt.y, layer_index))
			{
				have_one_who_hit = true;
				break;
			}
		}
	}

	//Note : on aurait pu très bien vérifier toutes les tuiles que touche le personnage d'un seul coup ; c'est une autre alternative ;)


	if(m_helper__error_at_last_call_of_isCollisionTile)
	{
		sm::lastError(sm::str("from 'sm::Map::hitCollisionTile()' : ")+sm::lastError());
		m_helper__error_at_last_call_of_hitCollisionTile = true;
	}

	return have_one_who_hit;
}









bool Map::hitCollisionTile(Entity& entity, short layer_index)
{
	for(int i=0; i< entity.getNbOfMembers(); i++)
	{
		if(hitCollisionTile(entity.member(i)))
			return true;
	}

	return false;
}











bool Map::willHitCollisionTile(Body& body, float total_pixel_incr_x, float total_pixel_incr_y, short layer_index, float* dist_x_before_hit__out, float* dist_y_before_hit__out)
{
	/*
		total_pixel_incr_xy sont : de combien réellement la position va changer sur tel axe dans la frame. E.g 2.6 pixel en x, 1.7 en y. 
		(Biensur allieurs, pour trouver à quelle pixel cela correspond on retire la partie décimale. Utiliser des réels pour les pixels 
		permet de bouger de moins d'un pixel par frame, permettant des pouvement plus lents). Ainsi l'utilisateur à le choix de mettre un décalge
		qui est la vitesse de son personnage, ou de faire correspondre cette vitesse avec le delta time par une multiplication, ou de mettre n'importe quelle valeur
		que son coeur désir. 
		Le tout ici est d'avoir 'de combien' précisément il va bouger de pixels.

		Par ailleurs cette fonction ne traite pas la trajectoire, seulement la destination : si une collision se trouve 
		entre le body et la destination mais que la destination est n'est pas une collision, le retour sera 'false'.
		Autrement dit attention au total_pixel_incr_xy > à la taille du bloc de destination. 
	*/

	Body copy = body;

	//Note : Les membres pointeurs ne sont pas copiés avec le constructeur de copie par défaut, les pointeurs de la copie pointent simplement sur les mêmes adresses que ceux de l'original.
	//Ainsi la ligne qui suit est fin de ne pas détruire la texture de 'body' lors de la sortie de fonction, qui va appeler destructeur de 'copy'.
		copy.setDestroyTextureInTheDestructor(false);

	copy.incrX(total_pixel_incr_x);
	copy.incrY(total_pixel_incr_y);



	bool hit = hitCollisionTile(copy, layer_index);

	if(m_helper__error_at_last_call_of_hitCollisionTile)
	{
		sm::lastError(sm::str("from 'sm::Map::willHitCollisionTile()' : ")+sm::lastError());
		return false;
	}



	if(hit) // = oui collision si mouvement efféctué.
	{
		if(dist_x_before_hit__out != nullptr)
		{
			bool go_to_left = (total_pixel_incr_x < 0 ? true : false);

			if(true) //Sans la partie décimal. Je vais garder celui-ci.
			{
				float body_pos_x = body.getX() + getStartX();

				if(not go_to_left)
					body_pos_x += body.getW();	//pour prendre en compte son côté droit plutôt

				int collision_tile_pos_x = static_cast<int>(body_pos_x + total_pixel_incr_x);
					collision_tile_pos_x /= getDestBLOCZ();	//pour trouver la colonne de tuile correspondante
					collision_tile_pos_x *= getDestBLOCZ();	//pour trouver le premier pixel correspondant à cette colonne
				
				if(go_to_left)
				{
					collision_tile_pos_x += getDestBLOCZ();	//pour décaler vers la droite de cette colonne puis que l'on va conger sa droite 
					*dist_x_before_hit__out = body_pos_x - static_cast<float>(collision_tile_pos_x);
				}		
				else
					*dist_x_before_hit__out = static_cast<float>(collision_tile_pos_x) - body_pos_x;

				if(*dist_x_before_hit__out < 0 or total_pixel_incr_x == 0)
					*dist_x_before_hit__out = 0;
			}

			if(false) //Avec la partie décimal. 
			{
				float body_pos_x = body.getX_f() + getStartX();

				if(not go_to_left)
					body_pos_x += body.getW();	//pour prendre en compte son côté droit plutôt

				int collision_tile_pos_x = static_cast<int>(body_pos_x + total_pixel_incr_x);
					collision_tile_pos_x /= getDestBLOCZ();	//pour trouver la colonne de tuile correspondante
					collision_tile_pos_x *= getDestBLOCZ();	//pour trouver le premier pixel correspondant à cette colonne
				
				if(go_to_left)
				{
					collision_tile_pos_x += getDestBLOCZ();	//pour décaler vers la droite de cette colonne puis que l'on va conger sa droite 
					*dist_x_before_hit__out = body_pos_x - static_cast<float>(collision_tile_pos_x);
				}		
				else
					*dist_x_before_hit__out = static_cast<float>(collision_tile_pos_x) - body_pos_x;

				if(*dist_x_before_hit__out < 0 or total_pixel_incr_x == 0)
					*dist_x_before_hit__out = 0;
			}
		}

		if(dist_y_before_hit__out != nullptr)
		{
			bool go_to_up = (total_pixel_incr_y < 0 ? true : false);
			
			{
				float body_pos_y = body.getY() + getStartY();

				if(not go_to_up)
					body_pos_y += body.getH();	//pour prendre en compte son côté inférieur plutôt

				int collision_tile_pos_y = static_cast<int>(body_pos_y + total_pixel_incr_y);
					collision_tile_pos_y /= getDestBLOCZ();	//pour trouver la ligne de tuile correspondante
					collision_tile_pos_y *= getDestBLOCZ();	//pour trouver le premier pixel correspondant à cette ligne
				
				if(go_to_up)
				{
					collision_tile_pos_y += getDestBLOCZ();	//pour décaler vers le bas de cette ligne puis que l'on va conger son bas 
					*dist_y_before_hit__out = body_pos_y - static_cast<float>(collision_tile_pos_y);
				}
				else
					*dist_y_before_hit__out = static_cast<float>(collision_tile_pos_y) - body_pos_y;

				if(*dist_y_before_hit__out < 0 or total_pixel_incr_y == 0)
					*dist_y_before_hit__out = 0;
			}
		}
	}
	else
	{
		if(dist_x_before_hit__out != nullptr)
			*dist_x_before_hit__out = 0;

		if(dist_y_before_hit__out != nullptr)
			*dist_y_before_hit__out = 0;

		// 0 puisqu'il n'y a pas eu de collision en vu.
	}

	return hit; 
}










bool Map::willHitCollisionTile(Entity& entity, float total_pixel_incr_x, float total_pixel_incr_y, short layer_index, float* dist_x_before_hit__out, float* dist_y_before_hit__out)
{
	//Ici on va simplement utiliser le précédent 'willHitCollisionTile()' avec chacun des membre du 'Entity'.
	
	// std::vector<Body> copies;
	// copies.resize(entity.getNbOfMembers());

	// for(size_t i=0; i< entity.getNbOfMembers(); i++)
	// {
	// 	copies.at(i) = entity.member(i);
	// 	copies.at(i).setDestroyTextureInTheDestructor(false);
	// }


	std::vector<float> dists_x_before_hit{};	//On va les stocker tous et renvoyer seulement le minimum d'entre eux.
	std::vector<float> dists_y_before_hit{};	

	bool have_one_who_will_hit = false;

	// for(Body& body : copies)
	for(int i=0; i< entity.getNbOfMembers(); i++)
	{
		Body& body = entity.member(i);

		float x_pix_before_hit = 0;
		float y_pix_before_hit = 0;

		if(willHitCollisionTile(body, total_pixel_incr_x, total_pixel_incr_y, layer_index, &x_pix_before_hit, &y_pix_before_hit))
		{
			have_one_who_will_hit = true;
			dists_x_before_hit.push_back(x_pix_before_hit);
			dists_y_before_hit.push_back(y_pix_before_hit);
		}
	}


	if(dist_x_before_hit__out != nullptr)
	{
		if(not dists_x_before_hit.empty())
			*dist_x_before_hit__out = *std::min_element(dists_x_before_hit.begin(), dists_x_before_hit.end());
			else 
				*dist_x_before_hit__out = 0; 
	}
	if(dist_y_before_hit__out != nullptr)
	{
		if(not dists_y_before_hit.empty())
			*dist_y_before_hit__out = *std::min_element(dists_y_before_hit.begin(), dists_y_before_hit.end());
			else 
				*dist_y_before_hit__out = 0; 
	}


	return have_one_who_will_hit;
}









bool Map::addCollisionTilesValues(unsigned short layer_index, std::vector<int> values)
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Warning|| in 'addCollisionTilesValue()' of '")+m_title+sm::str("', given layer index doesn't exist"));
		return false;
	}

	bool found = false;
	Vec_Int_VecInt* collision_lay_ptr=nullptr;

	for(Vec_Int_VecInt& collision_lay : m_collision_layers)
	{
		if(collision_lay.m_id == layer_index)
		{
			found = true;
			collision_lay_ptr = &collision_lay;
		}
	}

	if(not found or collision_lay_ptr == nullptr)
	{
		sm::lastError(sm::str("||Warning|| in 'addCollisionTilesValue()' of '")+m_title+sm::str("', layer ")+std::to_string(layer_index)+sm::str(" seems not set as a collision layer. "));
		return false;
	}


	{//Petit algorithme pour ne pas rajouter de doublons.

		std::vector<int>& lay_col_values = collision_lay_ptr->m_values;

		for(int& val : values)
		{
			std::vector<int>::iterator it;
			it = std::find(lay_col_values.begin(), lay_col_values.end(), val);

			if( it == lay_col_values.end() )
			{
				lay_col_values.push_back(val);
			} 
		}
	}

	return true;
}







bool Map::removeCollisionTilesValues(unsigned short layer_index, std::vector<int> values)
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Warning|| in 'removeCollisionTilesValue()' of '")+m_title+sm::str("', given layer index doesn't exist"));
		return false;
	}

	bool found = false;
	Vec_Int_VecInt* collision_lay_ptr=nullptr;

	for(Vec_Int_VecInt& collision_lay : m_collision_layers)
	{
		if(collision_lay.m_id == layer_index)
		{
			found = true;
			collision_lay_ptr = &collision_lay;
		}
	}

	if(not found or collision_lay_ptr == nullptr)
	{
		sm::lastError(sm::str("||Warning|| in 'removeCollisionTilesValue()' of '")+m_title+sm::str("', layer ")+std::to_string(layer_index)+sm::str(" seems not set as a collision layer. "));
		return false;
	}


	{
		std::vector<int>& lay_col_values = collision_lay_ptr->m_values;
		std::vector<int>::iterator it;

		for(const int& val : values)
		{
			it = std::find(lay_col_values.begin(), lay_col_values.end(), val);

			if(*it == val)
			{
				lay_col_values.erase(it);
			}
		}
	}

	return true;
}






std::vector<int> Map::getCollisionTilesValues(unsigned short layer_index)
{
	if(isInvalidLayerIndex(layer_index))
	{
		sm::lastError(sm::str("||Warning|| in 'getCollisionTilesValue()' of '")+m_title+sm::str("', given layer index doesn't exist"));
		return std::vector<int>();
	}

	bool found = false;
	Vec_Int_VecInt* collision_lay_ptr=nullptr;

	for(Vec_Int_VecInt& collision_lay : m_collision_layers)
	{
		if(collision_lay.m_id == layer_index)
		{
			found = true;
			collision_lay_ptr = &collision_lay;
		}
	}

	if(not found or collision_lay_ptr == nullptr)
	{
		sm::lastError(sm::str("||Warning|| in 'getCollisionTilesValue()' of '")+m_title+sm::str("', layer ")+std::to_string(layer_index)+sm::str(" seems not set as a collision layer. "));
		return std::vector<int>();
	}

	return collision_lay_ptr->m_values;
}





































bool Map::addBackgroundStack(uint16_t count)
{
	for(int i=0; i< count; i++)
		m_backgrounds_stacks.emplace_back();
	return true;
}



bool Map::popBackBackgroundStack()
{
	if(m_backgrounds_stacks.empty())
	{
		sm::lastError(std::format("||Warning|| in 'popBackBackgroundStack()' of '{}', no have any background stack to pop. ", m_title));
		return false;
	}

	m_backgrounds_stacks.pop_back();
	return true;
}


bool Map::popBackBackground(short stack_index)
{
	bool all_stacks=false;
	if(not INTERNAL_USE_stackIndexIsOk("popBackBackground()", stack_index, all_stacks))
		return false;
	
	if(all_stacks)
	{
		for(std::vector<Background>& backgrounds_stack : m_backgrounds_stacks)
			backgrounds_stack.pop_back();
	}
	else
		m_backgrounds_stacks.at(stack_index).pop_back();

	return true;
}




bool Map::isInvalidBackgroundStackIndex(short stack_index) const
{
	if(stack_index < 0 or stack_index >= m_backgrounds_stacks.size())
		return true;
	return false;
}

bool Map::isInvalidBackgroundIndex(short stack_index, short background_index)
{
	if(isInvalidBackgroundStackIndex(stack_index))
		return true;

	std::vector<Background>& backgrounds_stack = m_backgrounds_stacks.at(stack_index);

	if(background_index < 0 or (background_index > (int)backgrounds_stack.size()-1))	//attention à ne pas oublier la conversion en int car le retour de size() est size_t (= pas de négatif). Autre solution, faire un supérieur ou égal. 
		return true;
	return false;
}


bool Map::INTERNAL_USE_stackIndexIsOk(std::string function_name, short stack_index, bool& is_default_index) const
{
	if(m_backgrounds_stacks.empty())
	{
		sm::lastError(std::format("||Warning|| in '{}' of '{}', no have any background stack in this 'sm::Map'. ", function_name, m_title));
		return false;
	}

	is_default_index = false;
	if(stack_index == -1)
		is_default_index = true;

	else
	if(isInvalidBackgroundStackIndex(stack_index))
	{
		sm::lastError(std::format("||Warning|| in '{}' of '{}', invalid stack index. [given index : {}, max index : {}]. ", function_name, m_title, stack_index, (int)m_backgrounds_stacks.size()-1));
		return false;
	}

	return true;
}



bool Map::INTERNAL_USE_backgroundCooAreOk(std::string function_name, short stack_index, bool& is_default_stack_index, short background_index, bool& is_default_background_index)
{
	bool stack_is_ok = INTERNAL_USE_stackIndexIsOk(function_name, stack_index, is_default_stack_index);
	if(not stack_is_ok)
		return false;

	is_default_background_index = false;
	
	if(not is_default_stack_index)
	{
		std::vector<Background>& backgrounds_stack = m_backgrounds_stacks.at(stack_index);
		
		if(backgrounds_stack.empty()) 
		{
			sm::lastError(std::format("||Warning|| in '{}' of '{}', no have any background in the stack {}. ", function_name, m_title, stack_index));
			return false;
		}

		if(background_index == -1)
			is_default_background_index = true;

		else
		if(background_index < 0 or (background_index > (int)backgrounds_stack.size()-1))	//attention à ne pas oublier la conversion en int car le retour de size() est size_t (= pas de négatif). Autre solution, faire un supérieur ou égal. 
		{
			sm::lastError(std::format("||Warning|| in '{}' of '{}', invalid background index in the stack {}. [given index : {}, max index : {}]. ", function_name, m_title, stack_index, background_index, (int)backgrounds_stack.size()-1));
			return false;
		}
	}
	else
	{	
		int st_i = 0;
		for(auto& backgrounds_stack : m_backgrounds_stacks)
		{
			
			if(background_index == -1)
				is_default_background_index = true;

			else // if(background_index != -1)
			{
				if(backgrounds_stack.empty()) 
				{
					sm::lastError(std::format("||Warning|| in '{}' of '{}', required background index {} in all stacks, but the stack {} is empty. ", function_name, m_title, background_index, st_i));
					return false;
				}
			
				if(background_index < 0 or (background_index > (int)backgrounds_stack.size()-1))	//attention à ne pas oublier la conversion en int car le retour de size() est size_t (= pas de négatif). Autre solution, faire un supérieur ou égal. 
				{
					sm::lastError(std::format("||Warning|| in '{}' of '{}', required background index {} in all stacks, but is invalid in the stack {} witch max index is {}. ", function_name, m_title, background_index, st_i, (int)backgrounds_stack.size()-1));
					return false;
				}
			}

			st_i++;
		}
	}

	return true;
}






bool Map::addBackground(short stack_index, const char* file_path, Background_Mode mode, float sensibility_x, float sensibility_y, bool x_scrolling, bool y_scrolling)
{
	if(m_game == nullptr)
	{
		sm::lastError(sm::str("||Warning|| in 'addBackground()' of '")+m_title+sm::str("', cannot add : missing a valid 'sm::Game'. Make sure you have set a valid 'sm::Game'."));
		return false;
	}
	
	bool _skip_arg_;
	if(not INTERNAL_USE_stackIndexIsOk("addBackground()", stack_index, _skip_arg_))
	{
		sm::lastError(sm::lastError()+ sm::str("Try to add stack-s of background, or check the stack index if there are already stack-s in. "));
		return false;
	}


	std::vector<Background>& backgrounds_stack = m_backgrounds_stacks.at(stack_index);
	backgrounds_stack.emplace_back();
	Background& new_background = backgrounds_stack.back();
	

	new_background.m_image = m_game->loadImg(file_path);

	if(not new_background.m_image)
	{
		sm::lastError(std::format("from 'addBackground()' of '{}' : {}", m_title, sm::lastError()));
		return false;
	}

	SDL_QueryTexture(new_background.m_image, nullptr, nullptr, &new_background.m_w, &new_background.m_h);
	new_background.m_1st_image_file_path = file_path;
	new_background.m_mode=mode;
	new_background.m_sensibility_x = sensibility_x;
	new_background.m_sensibility_y = sensibility_y;
	new_background.m_x_scrolling = x_scrolling;
	new_background.m_y_scrolling = y_scrolling;
	new_background.m_is_init = true;

	

	return true;
}













bool Map::drawBackground(short stack_index, short background_index)	// -1 par défaut pour signifier de tout dessiner (dans l'ordre d'ajout)
{
	if(m_game == nullptr)
	{
		sm::lastError(std::format("||Warning|| in 'drawBackground()' of '{}', cannot add : missing a valid 'sm::Game'. Make sure you have set a valid 'sm::Game'. ", m_title));
		return false;
	}

	bool all_stacks=false;
	bool draw_all=false; 	//par stack
	if(not INTERNAL_USE_backgroundCooAreOk("drawBackground()", stack_index, all_stacks, background_index, draw_all))
		return false;

			
	std::vector<short> stack_indexes;
	if(not all_stacks)
		stack_indexes.push_back(stack_index);

	else
		for(int i=0; i< m_backgrounds_stacks.size(); i++)
			stack_indexes.push_back(i);



	for(short& st_i : stack_indexes)
	{
		std::vector<Background>& backgrounds_stack = m_backgrounds_stacks.at(st_i);


		std::vector<short> background_indexes;	//collecte des index à dessiner
		if(not draw_all)
			background_indexes.push_back(background_index);
		
		else
			for(size_t i=0; i< backgrounds_stack.size(); i++)
				background_indexes.push_back(i);
		


		for(short& i : background_indexes)
		{
			Background& bg = backgrounds_stack.at(i);

			if(bg.m_mode == FILL)
			{
				int res = SDL_RenderCopy(ThisGame->ren(), bg.m_image, nullptr, nullptr);
				if(res != 0)
				{
					sm::lastError(std::format("||Warning|| in 'drawBackground()' of '{}', [stack index : {}, background index : {}, first texture from '{}'], cannot draw : {}. ", m_title, st_i, i, bg.m_1st_image_file_path, SDL_GetError()));
					return false;
				}
			}
			else
			if(bg.m_mode == PARALLAX)
			{
				bg.m_param.current_dest_rect.x = bg.m_param.current_pos.x;
				bg.m_param.current_dest_rect.y = bg.m_param.current_pos.y;

				if(bg.m_param.mode == FILL_SCREEN)
				{
					bg.m_param.current_dest_rect.w = m_game->getRenW();
					bg.m_param.current_dest_rect.h = m_game->getRenH();
				}
				else
				if(bg.m_param.mode == ORIGINAL_SIZE)
				{
					SDL_QueryTexture(bg.m_image, nullptr, nullptr, 
						&bg.m_param.current_dest_rect.w,
						&bg.m_param.current_dest_rect.h);
				}
				else
				if(bg.m_param.mode == CUSTOM_SIZE)
				{
					bg.m_param.current_dest_rect.w = bg.m_param.custom_dest_rect.w;
					bg.m_param.current_dest_rect.h = bg.m_param.custom_dest_rect.h;
				}
				else
				if(bg.m_param.mode == FILL_SCREEN_IN_W)
				{
					SDL_QueryTexture(bg.m_image, nullptr, nullptr, &bg.m_w, &bg.m_h);

					double coef = double(m_game->getRenW()) / double(bg.m_w);
					bg.m_param.current_dest_rect.w = std::round(double(bg.m_w) * coef);
					bg.m_param.current_dest_rect.h = std::round(double(bg.m_h) * coef);
				}
				else
				if(bg.m_param.mode == FILL_SCREEN_IN_H)
				{
					SDL_QueryTexture(bg.m_image, nullptr, nullptr, &bg.m_w, &bg.m_h);

					double coef = double(m_game->getRenH()) / double(bg.m_h);
					bg.m_param.current_dest_rect.w = std::round(double(bg.m_w) * coef);
					bg.m_param.current_dest_rect.h = std::round(double(bg.m_h) * coef);
				}

				int res = SDL_RenderCopy(ThisGame->ren(), bg.m_image, nullptr, &bg.m_param.current_dest_rect);
				if(res != 0)
				{
					sm::lastError(std::format("||Warning|| in 'drawBackground()' of '{}', [stack index : {}, background index : {}, first texture from '{}'], cannot draw : {}. ", m_title, st_i, i, bg.m_1st_image_file_path, SDL_GetError()));
					return false;
				}
				else
				//partie de la mise en place de l'illusion
				{
					//Le but de l'algorithme est de remplir toute la bande x et/ou y par l'image.
					//C-à-d que lorsque l'image est dans une position où elle ne rempli pas toute la bande (soit car elle est trop petite, soit car elle scroll) on complète les trous (que se soit à gauche, à droite ou les deux pour la bande x, et pareilement avec le haut et le bas dans le cas de la bande y). 
					//Note : on peut traiter le scrolling dans les deux axes en même temps, ça ne gène pas.
					
					std::vector<int> bonus_render_copy_x; 	//Stocke les x des RenderCopy bonus pour combler les trous. Ainsi s'il y a également le scrolling y, on répétera ces RenderCopy pour chaque y de comblage.
		
					if(bg.m_x_scrolling)
					{
						SDL_Rect dest_rect = bg.m_param.current_dest_rect;
						int min_x = dest_rect.x;
						int max_x = dest_rect.x+dest_rect.w-1;

						//combler les potentiels trous à gauche
						for(int i=0; min_x > 0 and dest_rect.w > 0; i++)	//dest_rect.w > 0 sinon boucle infini
						{
							SDL_Rect tmp = {min_x-dest_rect.w, dest_rect.y, dest_rect.w, dest_rect.h};
							SDL_RenderCopy(ThisGame->ren(), bg.m_image, nullptr, &tmp);
							
							bonus_render_copy_x.push_back(tmp.x);
							min_x = tmp.x;

							if(i >= 20)
								break;	//Pour éviter une boucle trop grand on stoppe la boucle quoi qu'il arrive à ce stade car généralement avec un background de taille raisonable i tourne au alentours de 0 et 2. Si le programme vient en arriver à là c'est qu'il faut probablement revoir les dimensions du backgrounds car trop petites pour le rendu (du point du vu où c'est sensé recouvrir une grande surface).  
						}

						int ren_w = m_game->getRenW();

						//combler les potentiels trous à droite
						for(int i=0; max_x < ren_w-1 and dest_rect.w > 0; i++)	//dest_rect.w > 0 sinon boucle infini
						{
							SDL_Rect tmp = {max_x+1, dest_rect.y, dest_rect.w, dest_rect.h};
							SDL_RenderCopy(ThisGame->ren(), bg.m_image, nullptr, &tmp);
							
							bonus_render_copy_x.push_back(tmp.x);
							max_x = tmp.x+tmp.w-1;

							if(i >= 20)
								break;
						}


						if(bg.m_move_when_draw)
							moveBackground(bg.m_x_speed, 0.f, st_i, i);
							
						
					}





					if(bg.m_y_scrolling)
					{
						SDL_Rect dest_rect = bg.m_param.current_dest_rect;
						int min_y = dest_rect.y;
						int max_y = dest_rect.y+dest_rect.h-1;

						//combler les potentiels trous en haut
						for(int i=0; min_y > 0 and dest_rect.h > 0; i++)	//dest_rect.h > 0 sinon boucle infini
						{
							SDL_Rect tmp = {dest_rect.x, min_y-dest_rect.h, dest_rect.w, dest_rect.h};
							SDL_RenderCopy(ThisGame->ren(), bg.m_image, nullptr, &tmp);
								
								for(int& _x_ : bonus_render_copy_x)
								{
									SDL_Rect tmp2 = {_x_, tmp.y, dest_rect.w, dest_rect.h};
									SDL_RenderCopy(ThisGame->ren(), bg.m_image, nullptr, &tmp2);
								}

							min_y = tmp.y;

							if(i >= 20)
								break;	//Pour éviter une boucle trop grand on stoppe la boucle quoi qu'il arrive à ce stade car généralement avec un background de taille raisonable i tourne au alentours de 0 et 2. Si le programme vient en arriver à là c'est qu'il faut probablement revoir les dimensions du backgrounds car trop petites pour le rendu (du point du vu où c'est sensé recouvrir une grande surface).  
						}


						int ren_h = m_game->getRenH();

						//combler les potentiels trous en bas
						for(int i=0; max_y < ren_h-1 and dest_rect.h > 0; i++)	//dest_rect.h > 0 sinon boucle infini
						{
							SDL_Rect tmp = {dest_rect.x, max_y+1, dest_rect.w, dest_rect.h};
							SDL_RenderCopy(ThisGame->ren(), bg.m_image, nullptr, &tmp);
								
								for(int& _x_ : bonus_render_copy_x)
								{
									SDL_Rect tmp2 = {_x_, tmp.y, dest_rect.w, dest_rect.h};
									SDL_RenderCopy(ThisGame->ren(), bg.m_image, nullptr, &tmp2);
								}

							max_y = tmp.y+tmp.h-1;

							if(i >= 20)
								break;
						}


						if(bg.m_move_when_draw)	
							moveBackground(0.f, bg.m_y_speed, st_i, i);	
						
					}

			
				}
			}
		}
	}

	return true;
}







bool Map::moveBackground(float x_speed, float y_speed, short stack_index, short background_index)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("moveBackground()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	for(Background* bg_ptr : *given_bg_s)
	{
		bg_ptr->incrStartX(x_speed);
		bg_ptr->incrStartY(y_speed);
	}

	return true;
}


















std::vector<Background*>* Map::INTERNAL_USE_getTargetBackgrounds(std::string function_name, short stack_index, short background_index)
{
	bool all_stacks = false;
	bool edit_all = false; // = à l'échelle d'une stack
	if(not INTERNAL_USE_backgroundCooAreOk(function_name, stack_index, all_stacks, background_index, edit_all))
		return nullptr;

	m_helper_return_bg_array.clear();

	std::vector<int> stack_indexes;
	if(not all_stacks)
		stack_indexes.push_back(stack_index);

	else
		for(int i=0; i< m_backgrounds_stacks.size(); i++)
			stack_indexes.push_back(i);


	for(int& st_i : stack_indexes)
	{
		std::vector<Background>& backgrounds_stack = m_backgrounds_stacks.at(st_i);
		
		std::vector<short> background_indexes;	
		if(not edit_all)
			background_indexes.push_back(background_index);
		
		else
			for(size_t i=0; i< backgrounds_stack.size(); i++)
				background_indexes.push_back(i);
		

		for(short& i : background_indexes)
		{
			Background& bg = backgrounds_stack.at(i);
			
			m_helper_return_bg_array.push_back(&bg);
		}
	}

	return &m_helper_return_bg_array;
}






bool Map::changeBackgroundMode(short stack_index, short background_index, Background_Mode mode)
{
	bool all_stacks=false;
	bool change_all=false; 	
	if(not INTERNAL_USE_backgroundCooAreOk("changeBackgroundMode()", stack_index, all_stacks, background_index, change_all))
		return false;


	std::vector<short> stack_indexes;
	if(not all_stacks)
		stack_indexes.push_back(stack_index);

	else
		for(int i=0; i< m_backgrounds_stacks.size(); i++)
			stack_indexes.push_back(i);



	for(short& st_i : stack_indexes)
	{
		std::vector<Background>& backgrounds_stack = m_backgrounds_stacks.at(st_i);


		std::vector<short> background_indexes;	
		if(not change_all)
			background_indexes.push_back(background_index);
		
		else
			for(size_t i=0; i< backgrounds_stack.size(); i++)
				background_indexes.push_back(i);

		for(short& i : background_indexes)
		{
			Background& bg = backgrounds_stack.at(i);

			bg.m_mode = mode;
		}
	}

	return true;
}





bool Map::setBackgroundMoveWhenDraw(short stack_index, short background_index, bool new_state)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("setBackgroundMoveWhenDraw()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	for(Background* bg_ptr : *given_bg_s)
		bg_ptr->m_move_when_draw = new_state;
	
	return true;
}





















bool Map::setBackgroundSpeedX(short stack_index, short background_index, float val)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("setBackgroundSpeedX()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	for(Background* bg_ptr : *given_bg_s)
		bg_ptr->m_x_speed = val;

	return true;
}




bool Map::setBackgroundSpeedY(short stack_index, short background_index, float val)
{
	

	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("setBackgroundSpeedY()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	for(Background* bg_ptr : *given_bg_s)
		bg_ptr->m_y_speed = val;
	
	return true;
}





bool Map::setBackgroundSpeedXY(short stack_index, short background_index, float val_x, float val_y)
{
	bool success = true; 
	success = success & setBackgroundSpeedX(stack_index, background_index, val_x);
	success = success & setBackgroundSpeedY(stack_index, background_index, val_y);

	if(not success)
		sm::lastError(sm::str("from 'setBackgroundSpeedXY()' : ")+sm::lastError());
	
	return success;
}




bool Map::editBackgroundParallaxParam(
	short 					stack_index, short background_index, 
	Parallax_Param_Field 	field, 	int solo_val, SDL_Rect non_solo_vals)
{
	
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("editBackgroundParallaxParam()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	bool success = true;

	for(Background* bg_ptr : *given_bg_s)
	{
		Background& bg = *bg_ptr;
		
		if(bg.m_mode != PARALLAX) // On reste rigoureux ;)
		{
			sm::lastError(std::format("||Warning|| in 'editBackgroundParallaxParam()' of '{}', cannot edit : this background is not on paralax mode. Try to change it. ", m_title));
			return false;
		}

		Parallax_Param& param = bg.m_param;

		/* début des traitements */
		switch(field)
		{
			default : 
				success = false;
				sm::lastError(std::format("||Warning|| in 'editBackgroundParallaxParam()' of '{}', no parameter field correspond with given field. ", m_title));
				break;

			case APPEARANCE_MODE :
				try{
					param.mode = (Parallax_Dest_Appearance)solo_val;
				}
				catch(std::exception& er) 
				{ 
					success = false;
					sm::lastError(std::format("||Warning|| in 'editBackgroundParallaxParam()' of '{}', error occured when convert to `Paralax_Dest_Appearance' : {}", m_title, er.what()));
				}
				break;
				
			case CUSTOM_DEST_RECT_X :
				param.custom_dest_rect.x = solo_val;
				break;

			case CUSTOM_DEST_RECT_Y : 
				param.custom_dest_rect.y = solo_val;
				break;

			case CUSTOM_DEST_RECT_W :
				param.custom_dest_rect.w = solo_val;
				break;

			case CUSTOM_DEST_RECT_H : 
				param.custom_dest_rect.h = solo_val;
				break;

			case CURRENT_DEST_RECT_X :
				param.current_dest_rect.x = solo_val;
				break;

			case CURRENT_DEST_RECT_Y : 
				param.current_dest_rect.y = solo_val;
				break;

			case CURRENT_DEST_RECT_W :
				param.current_dest_rect.w = solo_val;
				break;

			case CURRENT_DEST_RECT_H :
				param.current_dest_rect.h = solo_val;
				break;

			case BEGIN_POS_X :
				param.begin_pos.x = solo_val;
				break;

			case BEGIN_POS_Y :
				param.begin_pos.y = solo_val;
				break;

			case CURRENT_POS_X :
				param.current_pos.x = solo_val;
				break;

			case CURRENT_POS_Y :
				param.current_pos.y = solo_val;
				break;

			case CUSTOM_DEST_RECT :
				param.custom_dest_rect = non_solo_vals;
				break;

			case CURRENT_DEST_RECT :
				param.current_dest_rect = non_solo_vals;
				break;

			case BEGIN_POS_XY : 
				param.begin_pos.x = non_solo_vals.x;
				param.begin_pos.y = non_solo_vals.y;
				break;

			case CURRENT_POS_XY :
				param.current_pos.x = non_solo_vals.x;
				param.current_pos.y = non_solo_vals.y;
				break;

			case CUSTOM_DEST_RECT_WH :
				param.custom_dest_rect.w = non_solo_vals.x;	//Histoire de n'avoir qu'à remplir le deux primiers champs lors de l'appel de la fonction : e.g editBackgroundParalaxParam(..WH, 0, {800, 480});
				param.custom_dest_rect.h = non_solo_vals.y;
				break;

			case CURRENT_DEST_RECT_WH :
				param.current_dest_rect.w = non_solo_vals.x;
				param.current_dest_rect.h = non_solo_vals.y;
				break;

			
		}//fin du switch

	}

	return success;
}








bool Map::setBackgroundSensibilityX(short stack_index, short background_index, float val)
{
	
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("setBackgroundSensibilityX()", stack_index, background_index);

	if(not given_bg_s)
		return false; //Les messages d'erreur sont déjà traités dans une fonction de la fonction précédente.

	for(Background* bg_ptr : *given_bg_s)
		bg_ptr->m_sensibility_x = val;

	return true;
}





bool Map::setBackgroundSensibilityY(short stack_index, short background_index, float val)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("setBackgroundSensibilityY()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	for(Background* bg_ptr : *given_bg_s)
		bg_ptr->m_sensibility_y = val;

	return true;
}




bool Map::setBackgroundSensibilityXY(short stack_index, short background_index, float val_x, float val_y)
{
	bool success = true;
	success = success & setBackgroundSensibilityX(stack_index, background_index, val_x);
	success = success & setBackgroundSensibilityY(stack_index, background_index, val_y);

	if(not success)
		sm::lastError(sm::str("from 'setBackgroundSensibilityXY()' : ")+sm::lastError());

	return success;
}







bool Map::setBackgroundScrollingXActiveState(short stack_index, short background_index, bool new_state)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("setBackgroundScrollingXActiveState()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	for(Background* bg_ptr : *given_bg_s)
		bg_ptr->m_x_scrolling = new_state;

	return true;
}




bool Map::setBackgroundScrollingYActiveState(short stack_index, short background_index, bool new_state)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("setBackgroundScrollingYActiveState()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	for(Background* bg_ptr : *given_bg_s)
		bg_ptr->m_y_scrolling = new_state;

	return true;
}



bool Map::setBackgroundScrollingXYActiveState(short stack_index, short background_index, bool new_state_x, bool new_state_y)
{
	bool success = true;
	success = success & setBackgroundScrollingXActiveState(stack_index, background_index, new_state_x);
	success = success & setBackgroundScrollingYActiveState(stack_index, background_index, new_state_y);

	if(not success)
		sm::lastError(sm::str("from 'setBackgroundScrollingXYActiveState()' : ")+sm::lastError());

	return success;
}



bool Map::setBackgroundImage(short stack_index, short background_index, SDL_Texture* texture, bool destroy_previous)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("setBackgroundImage()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	for(Background* bg_ptr : *given_bg_s)
	{
		if(destroy_previous)
		{
			if(bg_ptr->m_image)
				SDL_DestroyTexture(bg_ptr->m_image);
		}
		else
			::sm::addToClearAtTheEnd(bg_ptr->m_image);

		if(texture)
			bg_ptr->m_image = texture;
		else
		{
			sm::lastError(std::format("||Warning|| in 'setBackgroundImage()' of '{}', given texture is invalid. ", m_title));
			return false;
		}
	}

	return true;
}








bool Map::setBackgroundImage(short stack_index, short background_index, const char* file_path, bool destroy_previous)
{
	if(not m_game)
	{
		sm::lastError(std::format("||Warning|| in 'setBackgroundImage() [with file path]' of '{}', cannot set : missing a valid 'sm::Game'. Make sure you have set a valid 'sm::Game'. ", m_title));
		return false;
	}

	SDL_Texture* tex = m_game->loadImg(file_path);

	if(not tex)
	{
		sm::lastError(std::format("||Warning|| in 'setBackgroundImage() [with file path]' of '{}', image load failled : {} ", m_title, sm::lastError()));
		return false;
	}

	return setBackgroundImage(stack_index, background_index, tex, destroy_previous);
}




/*************************************************************************\
	*******************************************************************
		***********************************************************
			***************************************************
				*******************************************
				*******************************************
				*******************************************
			***************************************************
		***********************************************************
	*******************************************************************
/**************************************************************************/		





short Map::getNbOfBackgroundStacks() const
{
	return static_cast<short>(m_backgrounds_stacks.size());
}


short Map::getNbOfBackgrounds(unsigned short stack_index) const
{
	if(isInvalidBackgroundStackIndex(stack_index))
	{
		sm::lastError(std::format("||Warning|| in 'getNbOfBackgrounds()' of '{}', invalid stack index. [given index : {}, max index : {}]. ", m_title, stack_index, (int)m_backgrounds_stacks.size()-1));
		return 0;
	}

	return static_cast<short>(m_backgrounds_stacks.at(stack_index).size());
}


Parallax_Param* Map::getBackgroundParallaxParam(unsigned short stack_index, unsigned short background_index)
{
	bool _skip_arg_, _skip_arg2; 	
	if(not INTERNAL_USE_backgroundCooAreOk("getBackgroundParallaxParam()", stack_index, _skip_arg_, background_index, _skip_arg2))
		return nullptr;

	return &m_backgrounds_stacks.at(stack_index).at(background_index).m_param;
}



SDL_Rect Map::getBackgroundParallaxParam(unsigned short stack_index, unsigned short background_index, Parallax_Param_Field field)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("getBackgroundParallaxParam()", stack_index, background_index);

	SDL_Rect return_val = {0,0,0,0};

	if(not given_bg_s)
		return {0,0,-1,-1};

	bool success = true;

	/*
	Pour les valeurs solo : le retour est stocké dans le 'x'; les autres champs seront nulles. E.g : getBackgroundParallaxParam(0, 2, sm::BEGIN_POS_Y).x;
	pour les valeurs duo : les retours sont stockés dans les champs 'x' et 'y';
	Pour les rectangles complets : la valeur retourné est le rectangle demandé tout simplement.
	*/
	

	for(Background* bg_ptr : *given_bg_s)
	{
		Background& bg = *bg_ptr;
		
		if(bg.m_mode != PARALLAX) // On reste rigoureux ;)
		{
			sm::lastError(std::format("||Warning|| in 'getBackgroundParallaxParam()' of '{}', this background is not on paralax mode. Try to change it. ", m_title));
			return {0,0,-1,-1};
		}

		Parallax_Param& param = bg.m_param;

		/* début des traitements */
		switch(field)
		{
			default : 
				success = false;
				sm::lastError(std::format("||Warning|| in 'getBackgroundParallaxParam()' of '{}', no parameter field correspond with given field. ", m_title));
				break;

			case APPEARANCE_MODE :
				return_val.x = (int)param.mode;
				break;
				
			case CUSTOM_DEST_RECT_X :
				return_val.x = param.custom_dest_rect.x;
				break;

			case CUSTOM_DEST_RECT_Y : 
				return_val.x = param.custom_dest_rect.y;
				break;

			case CUSTOM_DEST_RECT_W :
				return_val.x = param.custom_dest_rect.w;
				break;

			case CUSTOM_DEST_RECT_H : 
				return_val.x = param.custom_dest_rect.h;
				break;

			case CURRENT_DEST_RECT_X :
				return_val.x = param.current_dest_rect.x;
				break;

			case CURRENT_DEST_RECT_Y : 
				return_val.x = param.current_dest_rect.y;
				break;

			case CURRENT_DEST_RECT_W :
				return_val.x = param.current_dest_rect.w;
				break;

			case CURRENT_DEST_RECT_H :
				return_val.x = param.current_dest_rect.h;
				break;

			case BEGIN_POS_X :
				return_val.x = param.begin_pos.x;
				break;

			case BEGIN_POS_Y :
				return_val.x = param.begin_pos.y;
				break;

			case CURRENT_POS_X :
				return_val.x = param.current_pos.x;
				break;

			case CURRENT_POS_Y :
				return_val.x = param.current_pos.y;
				break;

			case CUSTOM_DEST_RECT :
				return_val = param.custom_dest_rect;
				break;

			case CURRENT_DEST_RECT :
				return_val = param.current_dest_rect;
				break;

			case BEGIN_POS_XY : 
				return_val.x = param.begin_pos.x;
				return_val.y = param.begin_pos.y;
				break;

			case CURRENT_POS_XY :
				return_val.x = param.current_pos.x;
				return_val.y = param.current_pos.y;
				break;

			case CUSTOM_DEST_RECT_WH :
				return_val.x = param.custom_dest_rect.w ;	
				return_val.y = param.custom_dest_rect.h ;
				break;

			case CURRENT_DEST_RECT_WH :
				return_val.x = param.current_dest_rect.w;
				return_val.y = param.current_dest_rect.h;
				break;

			
		}//fin du switch

	}

	if(not success)
		return {0,0,-1,-1};

	return return_val;
}








bool Map::getBackgroundMoveWhenDraw(unsigned short stack_index, unsigned short background_index)
{	
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("getBackgroundMoveWhenDraw()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	Background* bg_ptr = given_bg_s->front();
	return bg_ptr->m_move_when_draw;
}



float Map::getBackgroundSpeedX(unsigned short stack_index, unsigned short background_index)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("getBackgroundSpeedX()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	Background* bg_ptr = given_bg_s->front();
	return bg_ptr->m_x_speed;
}



float Map::getBackgroundSpeedY(unsigned short stack_index, unsigned short background_index)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("getBackgroundSpeedY()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	Background* bg_ptr = given_bg_s->front();
	return bg_ptr->m_y_speed;
}









float Map::getBackgroundSensibilityX(unsigned short stack_index, unsigned short background_index)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("getBackgroundSensibilityX()", stack_index, background_index);

	if(not given_bg_s)
		return false; 

	Background* bg_ptr = given_bg_s->front();
	return bg_ptr->m_sensibility_x;
}




float Map::getBackgroundSensibilityY(unsigned short stack_index, unsigned short background_index)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("getBackgroundSensibilityY()", stack_index, background_index);

	if(not given_bg_s)
		return false; 

	Background* bg_ptr = given_bg_s->front();
	return bg_ptr->m_sensibility_y;
}






bool Map::getBackgroundScrollingXActiveState(unsigned short stack_index, unsigned short background_index)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("getBackgroundScrollingXActiveState()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	Background* bg_ptr = given_bg_s->front();
	return bg_ptr->m_x_scrolling;
}




bool Map::getBackgroundScrollingYActiveState(unsigned short stack_index, unsigned short background_index)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("getBackgroundScrollingYActiveState()", stack_index, background_index);

	if(not given_bg_s)
		return false;

	Background* bg_ptr = given_bg_s->front();
	return bg_ptr->m_y_scrolling;
}




SDL_Texture* Map::getBackgroundImage(unsigned short stack_index, unsigned short background_index)
{
	std::vector<Background*>* 
	given_bg_s = INTERNAL_USE_getTargetBackgrounds("getBackgroundImage()", stack_index, background_index);

	if(not given_bg_s)
		return nullptr;

	Background* bg_ptr = given_bg_s->front();
	return bg_ptr->m_image;
}






}; //fin du namespace sm

