#include "SDL3_GSlib.hpp"
#include "SDL3_GSlib-Geo.hpp"
#include <random>
#include <bitset>


//Version modifiée le 28/10/25 (à moins d'oubli de modifier cette ligne ou autre).

gs::Game *ThisGame = nullptr; 


std::vector<SDL_Texture*> texturesToClearAtTheEnd{};
std::vector<SDL_Renderer*> renderersToClearAtTheEnd{}; 
std::vector<SDL_PixelFormat*> pixelFormatsToClearAtTheEnd{};
std::vector<std::vector<char>> paths_loaded_with_gsloadImg{};


std::ostream& operator<<(std::ostream& os, SDL_Rect& rect)
{
	os << "[x "<<rect.x<<"| y "<<rect.y<<"| w "<<rect.w<<"| h "<<rect.h<<"]";
	return os;
}

std::ostream& operator<<(std::ostream& os, SDL_FRect& rect)
{
	os << "[x "<<rect.x<<"| y "<<rect.y<<"| w "<<rect.w<<"| h "<<rect.h<<"]";
	return os;
}

std::ostream& operator<<(std::ostream& os, SDL_Point& point)
{
	os << "[x "<<point.x<<"| y "<<point.y<<"]";
	return os;
}

std::ostream& operator<<(std::ostream& os, SDL_FPoint& point)
{
	os << "[x "<<point.x<<"| y "<<point.y<<"]";
	return os;
}

std::ostream& operator<<(std::ostream& os, SDL_Color& col)
{
	os << "[r "<<int(col.r)<<"| g "<<int(col.g)<<"| b "<<int(col.b)<<"| a "<<int(col.a)<<"]";
	return os;
}

bool operator==(const SDL_Rect& rect1, const SDL_Rect& rect2)
{
	return 
	((rect1.x == rect2.x) && (rect1.y == rect2.y) 
	&& (rect1.w == rect2.w) && (rect1.h == rect2.h));
}

bool operator==(const SDL_FRect& rect1, const SDL_FRect& rect2)
{
	return 
	((rect1.x == rect2.x) && (rect1.y == rect2.y) 
	&& (rect1.w == rect2.w) && (rect1.h == rect2.h));
}

bool operator==(const SDL_Point& pt1, const SDL_Point& pt2)
{
	return ( (pt1.x == pt2.x) && (pt1.y == pt2.y) );
}

bool operator==(const SDL_FPoint& pt1, const SDL_FPoint& pt2)
{
	return ( (pt1.x == pt2.x) && (pt1.y == pt2.y) );
}

bool operator==(const SDL_Color& col1, const SDL_Color& col2)
{
	return 
	((col1.r == col2.r) && (col1.g == col2.g) 
	&& (col1.b == col2.b) && (col1.a == col2.a));
}








namespace gs
{

void ________________FONCTION2_____(){}
void ________________FONCTION3_____(){}

bool game_started = false;
bool game_won = false;
bool end_game = false;
bool show_comments = false;
std::string _lastError_;

void initFreeResources()
{
	std::atexit(freeGivenResources);
}


bool& Input::operator[](std::string id)
{
	if(m_inputs.contains(id))
		return m_inputs[id];

	m_inputs[id] = false;
	return m_inputs[id];
}





bool Input::operator[](std::string id, bool new_state)
{
	m_inputs[id] = new_state;
	return new_state;
}









bool Mouse_Tracker::isPreviouslyDown() const {return m_previously_down;}

bool Mouse_Tracker::isDown() const {return m_is_down;}

Vec2f Mouse_Tracker::getPosWhenDown() const {return m_pos_when_down;}

Vec2f Mouse_Tracker::getPosWhenUp() const {return m_pos_when_up;}

Vec2f Mouse_Tracker::getPos() const {return m_pos;}

bool Mouse_Tracker::moved() const {return m_prev_pos != m_pos;}

bool Mouse_Tracker::isMultiClick(char* out__clicks) const 
{
	if(out__clicks) 
		*out__clicks=m_clicks_count; 
	
	return m_is_multi_click;
}

bool Mouse_Tracker::isDoubleClick() const {return m_is_multi_click and m_clicks_count==2;}

void Mouse_Tracker::incrMultiClickCount(char val) 
{
	m_clicks_count += val; 
	
	if(m_clicks_count < 0) 
		m_clicks_count = 0;
}


void Mouse_Tracker::setTargetBtn(bool is_left) {m_target_btn_is_left = is_left;}

bool Mouse_Tracker::targetBtnIsLeft() const {return m_target_btn_is_left;}

bool Mouse_Tracker::clickedDown() const {return m_is_down and not m_previously_down;}

bool Mouse_Tracker::clickedUp() const {return not m_is_down and m_previously_down;}


	
void Mouse_Tracker::processEvent(SDL_Event& ev)
{
	Uint8 target_btn = SDL_BUTTON_LEFT;
	if(not m_target_btn_is_left) 
		target_btn = SDL_BUTTON_RIGHT;
	
	if(ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN and ev.button.button == target_btn)
	{
		m_is_down = true;
		m_pos_when_down.x = ev.button.x;
		m_pos_when_down.y = ev.button.y;
		
		if(ev.button.clicks >= 2)
			m_is_multi_click = true;
		else
			m_is_multi_click = false;
		
		m_clicks_count = ev.button.clicks;
	}
	else 
	if(ev.type == SDL_EVENT_MOUSE_BUTTON_UP and ev.button.button == target_btn)
	{
		m_is_down = false;
		m_is_multi_click = false;
		m_clicks_count = 0;
		
		m_pos_when_up.x = ev.button.x;
		m_pos_when_up.y = ev.button.y;
	}
		
		
	m_prev_pos = m_pos;	
		
	//Pour la position courante:
	SDL_GetMouseState(&m_pos.x, &m_pos.y);
}



void Mouse_Tracker::update()
{
	if(m_next_time_operation)
	{
		if(m_next_time_operation == 1)
			m_previously_down = true;
		else
		if(m_next_time_operation == 2)
			m_previously_down = false;
		
		m_next_time_operation = 0;
	}
	
	if(m_is_down and not m_previously_down)
		m_next_time_operation = 1;
	else 
	if(not m_is_down and m_previously_down)
		m_next_time_operation = 2;
	
}
	






//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//___________________________FONCTIONS  DE  GAME______________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________


Game::Game()
: //mixActive_(false), initMix_is_allowed(true),
//free_channel_(0), allocated_channels(0), 
dt_(0), 
lastTime(0) 
// lastTime(SDL_GetTicks()), 
{
	// initFreeResources();
	std::atexit(freeGivenResources);
};


//____________________________________________________________________________________


Game::~Game()
{
	if(ren_ != nullptr)
		SDL_DestroyRenderer(ren_); ren_ = nullptr;

	if(win_ != nullptr)
		SDL_DestroyWindow(win_);   win_ = nullptr;

	
	//if(mixActive_) 
		//Mix_CloseAudio();


	if(win_icon != nullptr)
		SDL_DestroySurface(win_icon);

	if(is_main_game)
		SDL_Quit();
	else
		std::atexit(SDL_Quit);

	if(show_comments)
	std::cout<<"\tEnd Game succes" << std::endl;
};



//____________________________________________________________________________________
//____________________________________________________________________________________



void Game::InitSDLEngine(int window_w, int window_h, const char* window_title, Uint32 win_flags)
{
	if(isInit) return;

	if(not SDL_Init(SDL_INIT_VIDEO))
	{
		std::string error = std::string("||Fatal Error|| in 'gs::Game::InitSDLEngine()', 'SDL_Init()' failled : ")+std::string(SDL_GetError());
		throw std::runtime_error(error);
	};

	win_ = SDL_CreateWindow(window_title, window_w, window_h, win_flags); 
	if(win_ == nullptr)
	{
		SDL_Quit();
		// std::exit(-1);
		std::string error = std::string("||Fatal Error|| in 'gs::Game::InitSDLEngine()', 'SDL_CreateWindow()' failled : ")+std::string(SDL_GetError());
		throw std::runtime_error(error);
	};

	ren_ = SDL_CreateRenderer(win_, nullptr);
	if(ren_ == nullptr)
	{
		SDL_DestroyWindow(win_);
		SDL_Quit();
		// std::exit(-1);
		std::string error = std::string("||Fatal Error|| in 'gs::Game::InitSDLEngine()', 'SDL_CreateRenderer()' failled : ")+std::string(SDL_GetError());
		throw std::runtime_error(error);
	};

	SDL_SetRenderDrawBlendMode(ren_, SDL_BLENDMODE_BLEND); //Pour les primitives.
	SDL_SetRenderDrawColor(ren_, 0, 0, 0, 255); //Pour les primitives.

	if(show_comments)
	std::cout<<"\tSDL and Game init succes" << std::endl;
	
	Win_h = window_h;
	Win_w = window_w;
	isInit = true;
}



//____________________________________________________________________________________
//____________________________________________________________________________________



bool Game::InitByAssociateTo(SDL_Window *W, SDL_Renderer *newR )
{
	if(not isInit) 
	{
		isInit = true; 
		win_ = W;
		changeRenderer(newR);

		if(show_comments)
		std::cout<<"\tGame launched" << std::endl;
	}
	else
	{
		if(show_comments)
		std::cout <<"\tGame already launched" << std::endl;

		return false;
	}; 

	return true;
};



//____________________________________________________________________________________
//____________________________________________________________________________________

		/*simple geters*/
//----------------------------------------------
SDL_Renderer *Game::ren() const {return ren_;};
//----------------------------------------------
SDL_Window *Game::win() const {return win_;}; 
//----------------------------------------------
int Game::getWinW() 
{
	SDL_GetWindowSize(win(), &Win_w, &Win_h);
	return Win_w;
};
//----------------------------------------------
int Game::getWinH() 
{
	SDL_GetWindowSize(win(), &Win_w, &Win_h);
	return Win_h;
};
//----------------------------------------------
int Game::getRenW() 
{
	SDL_GetCurrentRenderOutputSize(ren(), &m_ren_w, &m_ren_h);
	return m_ren_w;
};
//----------------------------------------------
int Game::getRenH() 
{
	SDL_GetCurrentRenderOutputSize(ren(), &m_ren_w, &m_ren_h);
	return m_ren_h;
};
//----------------------------------------------
float Game::getRenW_f()
{
	return float(getRenW());
} 
//----------------------------------------------
float Game::getRenH_f()
{
	return float(getRenH());
}
//----------------------------------------------
//bool Game::mixActive() const {return mixActive_;};
//----------------------------------------------
std::string Game::lastError() const { return lastError();};

double Game::dt() const { return dt_;};


//____________________________________________________________________________________


void Game::lastError(std::string error) {lastError(error);}

void Game::setWinSize(int w, int h) { Win_h = h; Win_w = w; }


//____________________________________________________________________________________
//____________________________________________________________________________________



void Game::changeRenderer(SDL_Renderer *newR, bool destroy) 
{
	if(destroy)
		if(ren_ != nullptr) 
			SDL_DestroyRenderer(ren_); 

	if(not destroy)
		addToClearAtTheEnd(ren_);

	ren_ = newR;
};
		


//____________________________________________________________________________________
//____________________________________________________________________________________



SDL_Texture* Game::loadImg(const char *pth)
{
	if(not isInit)
	{
		gs::lastError("||Warning|| in 'Game::loadImg()', not yet init. ");
		return nullptr;
	}

	SDL_Texture* res = ::gs::loadImg(ren_, pth);
	if(res == nullptr)
	{
		gs::lastError(gs::str("from 'gs::Game::loadImg()' : ") +gs::lastError());	
	}
	return res;
};		



//____________________________________________________________________________________
//____________________________________________________________________________________



bool Game::drawTile(SDL_Texture *tileset, int x, int y, int dest_w, int dest_h, int srcX, int srcY, int src_w, int src_h, bool isRenderTheTarget)
{
	return gs::drawTile(ren_, nullptr, tileset, x, y, dest_w, dest_h, srcX, srcY, src_w, src_h, isRenderTheTarget);
};



//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::fpsProcessBegin()
{
	m_frame_begin_ticks = SDL_GetTicks();
	m_fpsProcessBegin_is_ok = true;
}



void Game::fpsProcessEnd(Uint64 fps) 
{
	if(not m_fpsProcessBegin_is_ok)
	{
		gs::lastError("!! in 'Game::fpsProcessEnd()', a call of 'Game::fpsProcessBegin()' is needed before calling 'Game::fpsProcessEnd()', as many times as 'Game::fpsProcessEnd()' is called. ");
		return;
	}

	if(fps == 0) 
		fps = 1;
	
	double FPS = static_cast<double>(fps);
	double one_frame_duration = 1.0/FPS; //En sec
	uint64_t one_frame_ms_duration = std::lround(one_frame_duration * 1000.0);

	m_frame_end_ticks = SDL_GetTicks();
	
	uint64_t delta = m_frame_end_ticks - m_frame_begin_ticks;

	if(delta < one_frame_ms_duration)
		SDL_Delay(one_frame_ms_duration - delta);

	m_fpsProcessBegin_is_ok = false;
	
}



//____________________________________________________________________________________
//____________________________________________________________________________________



void Game::processDeltaTime()
{
	//#if WITH_MILLISEC

	//if(lastTime == 0) lastTime = SDL_GetTicks();

	//double nowTime = SDL_GetTicks();
	//dt_ = (nowTime - lastTime)*0.001; // Conversion en secondes.
	//lastTime = nowTime; //SDL_GetTicks();



	//#else //Avec nanoseconde

	if(m_last_time_ns == 0) m_last_time_ns  = SDL_GetTicksNS();

	uint64_t now_time_ns = SDL_GetTicksNS();
	dt_ = static_cast<double>(now_time_ns - m_last_time_ns)*0.000000001; // Conversion en secondes.
	m_last_time_ns = now_time_ns;

	//#endif
}


//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::addToClearAtTheEnd(SDL_Texture* tex)
{
	::gs::addToClearAtTheEnd(tex);
}

//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::addToClearAtTheEnd(SDL_Renderer* renderer)
{
	::gs::addToClearAtTheEnd(renderer);
}



//____________________________________________________________________________________
//____________________________________________________________________________________

Uint32 Game::getWinID() const
{
	if(win_ == nullptr)
		return 0;

	return SDL_GetWindowID(win());
}



//____________________________________________________________________________________
//____________________________________________________________________________________


bool Game::isCloseWinEvent(SDL_Event& ev)
{
	if(ev.type == SDL_EVENT_QUIT)
		return true;

	if(ev.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
		if(getWinID() == ev.window.windowID)
			return true;

	return false;
}


//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::setWinIcon(const char* path)
{
	if(not isInit)
	{
		gs::lastError("||Warning|| in 'Game::setWinIcon()', not yet init. ");
		return;
	}

	if(win_icon != nullptr)
		SDL_DestroySurface(win_icon);

	win_icon = IMG_Load(path); 

	SDL_SetWindowIcon(win(), win_icon);
}

//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::setWinTitle(const char* new_title)
{
	if(win_ != nullptr and new_title != nullptr)
	{
		SDL_SetWindowTitle(win_, new_title);
	}
}

//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::setAsMainGame()
{
	is_main_game = true;
}

//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::setIsMainGame(const bool b)
{
	is_main_game = b;
}


void Game::setUserData(void* data)
{
	m_user_data = data;
}

void* Game::getUserData()
{
	return m_user_data;
}

//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::renPresent()
{
	SDL_RenderPresent(ren());
}


void Game::renClear()
{
	SDL_RenderClear(ren());
}


//____________________________________________________________________________________
//____________________________________________________________________________________


SDL_Color Game::setDrawColor(SDL_Color new_color)
{
	//return : Renvoie la couleur précédente.

	SDL_Color curr_col;

	if(not isInit)
	{
		gs::lastError("||Warning|| in 'Game::setDrawColor()', not yet init. ");
		return curr_col;
	}

	SDL_GetRenderDrawColor(ren_, &curr_col.r, &curr_col.g, &curr_col.b, &curr_col.a);
	SDL_SetRenderDrawColor(ren_, new_color.r, new_color.g, new_color.b, new_color.a);

	return curr_col;
}


SDL_Color Game::getDrawColor()
{
	SDL_Color curr_col;

	if(not isInit)
	{
		gs::lastError("||Warning|| in 'Game::getDrawColor()', not yet init. ");
		return curr_col;
	}

	SDL_GetRenderDrawColor(ren_, &curr_col.r, &curr_col.g, &curr_col.b, &curr_col.a);
	return curr_col;
}





//____________________________________________________________________________________
//____________________________________________________________________________________



int Game::renX(float percent)
{
	return int(float(ThisGame->getRenW()) * percent);
}

		
int Game::renY(float percent)
{
	return int(float(ThisGame->getRenH()) * percent);
}












//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//___________________________________UTILS____________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________







//===================================================================================//










SDL_Texture* loadImg(SDL_Renderer *renderer, const char* path)
{
	SDL_Texture *texture = nullptr;
    texture = IMG_LoadTexture(renderer, path);
   
   
    if(not texture)
    {
    	lastError(std::string("||Error|| in 'gs::loadImg()' : cannot load the image from '") 
    		+ std::string(path) 
    		+ std::string("', : ") 
    		+ std::string(SDL_GetError()));
   
    	return nullptr;
    }
    else 	//Mise en place dans la pile de nettoyage + établissement de la provenance en tant qu'info.
    {
    	{
    		paths_loaded_with_gsloadImg.emplace_back();
	    	std::vector<char>& vec_str = paths_loaded_with_gsloadImg.back();

	    	std::string for_size = path;
	    	vec_str.resize(for_size.size()+1);
	    	vec_str.back() = '\0';

	    	for(int i=0; i< vec_str.size(); i++)
	    	{
	    		char c = (char)path[i];
	    		vec_str.at(i) = c;
	    	}
	    	
	    	const char* source_file_as_userdata = paths_loaded_with_gsloadImg.back().data();
	        
	        //SDL_SetTextureUserData(texture, (void*)source_file_as_userdata);    
		}

		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		::gs::addToClearAtTheEnd(texture);
    }

	return texture;
}



//____________________________________________________________________________________
//____________________________________________________________________________________



SDL_Texture *loadImgAndDeleteAColor(SDL_Renderer *renderer, const char *p, Color C)
{
	SDL_Surface *loadedImage = nullptr; SDL_Texture *texture = nullptr;
    loadedImage = IMG_Load(p);

    SDL_SetSurfaceColorKey(loadedImage, true, SDL_MapRGB(SDL_GetPixelFormatDetails(loadedImage->format), SDL_GetSurfacePalette(loadedImage), C.r, C.g, C.b));

    if (loadedImage != nullptr)
    {
        texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
        SDL_DestroySurface(loadedImage);
        loadedImage = nullptr;
    }
    else
    printf("  L'image n'a pas pu être chargée! SDL_Error : %s\n", SDL_GetError());
 
    return texture;
}



//____________________________________________________________________________________
//____________________________________________________________________________________


void logMessage(const char *msg, const char *_getError, bool isFatalError)
{
	if(_getError != nullptr)
		std::cout << msg << " : " << _getError << std::endl;
	else
		std::cout << msg << std::endl;

	if(isFatalError)
		exit(-1);
}

//____________________________________________________________________________________
//____________________________________________________________________________________


bool drawTile(SDL_Renderer* ren, SDL_Texture* target, SDL_Texture* tileset, 
					int x, int y, int dest_w, int dest_h, int srcX, int srcY, int src_w, 
					int src_h, bool takeInToAccountTheTarget)
{
	SDL_FRect src, dest;
	bool success = true;

	src.x = srcX;		src.y = srcY;		src.w = src_w;		src.h = src_h;

	dest.x = x;		    dest.y = y;		    dest.w = dest_w;		dest.h = dest_h; 


	SDL_Texture* previousTarget = SDL_GetRenderTarget(ren);

	if(takeInToAccountTheTarget)
		SDL_SetRenderTarget(ren, target);

	if(not SDL_RenderTexture(ren, tileset, &src, &dest))
	{
		lastError(std::string("||Warnig|| in `gs::drawTile()' , impossible to draw the tile to [") 
			+std::to_string(x)  
			+std::string(" ; ")
			+std::to_string(y)
			+std::string("], : ")
			+std::string(SDL_GetError()));

		success = false;
	}

	if(takeInToAccountTheTarget)
		SDL_SetRenderTarget(ren, previousTarget);

	return success;
	
};

//____________________________________________________________________________________
//____________________________________________________________________________________


void addToClearAtTheEnd(SDL_Texture* tex)
{
	for(SDL_Texture* t : texturesToClearAtTheEnd)
		if(t == tex)
			return;
	if(tex)
	texturesToClearAtTheEnd.push_back(tex);
}


void addToClearAtTheEnd(SDL_Renderer* ren)
{
	for(SDL_Renderer* r : renderersToClearAtTheEnd)
		if(r == ren)
			return;
	if(ren)
	renderersToClearAtTheEnd.push_back(ren);
}


//void addToClearAtTheEnd(SDL_PixelFormat* fmt)
//{
//	for(SDL_PixelFormat* f : pixelFormatsToClearAtTheEnd)
//		if(f == fmt)
//			return;
//	if(fmt)	
//	pixelFormatsToClearAtTheEnd.push_back(fmt);
//}


void freeGivenResources()
{
	if(show_comments)
	std::cout << "\n|______Free ress______|\n";

	for(SDL_Texture* tex : texturesToClearAtTheEnd)
	{
		if(tex)
		{
			if(show_comments)
			{
				//std::string name;
				//if((char*)SDL_GetTextureUserData(tex))
				//	name = (char*)SDL_GetTextureUserData(tex);
				//else
				//	name = "(cannot get title)";
	
				//std::printf("- texture : info -> %s, addr -> %p\n", name.c_str(), tex);
			}
			
			SDL_DestroyTexture(tex);
		}
		/*else std::printf("*\n");*/
	}
	texturesToClearAtTheEnd.clear();
	texturesToClearAtTheEnd.shrink_to_fit();

	
	for(SDL_Renderer* renderer : renderersToClearAtTheEnd)
	{
		if(renderer)
		{
			if(show_comments)
			{
				std::printf("- renderer : info -> %s, addr -> %p\n", SDL_GetRendererName(renderer), renderer);
			}
			
			SDL_DestroyRenderer(renderer);
		}
	}
	renderersToClearAtTheEnd.clear();
	renderersToClearAtTheEnd.shrink_to_fit();


	//for(SDL_PixelFormat* fmt : pixelFormatsToClearAtTheEnd)
	//{
	//	if(fmt)
	//	{
	//		if(show_comments)
	//			std::printf("- pixel format, addr -> %p\n", fmt);

	//		SDL_FreeFormat(fmt);
	//	}		
	//}
	//pixelFormatsToClearAtTheEnd.clear();
	//pixelFormatsToClearAtTheEnd.shrink_to_fit();
	
	if(show_comments)
	std::cout << "|____Free ress end____|\n";
}


//____________________________________________________________________________________
//____________________________________________________________________________________


double rad(double degre)
{
	return ( degre * 2.0*_pi_) / 360.0;
};

/*-------------------------------*/

double degree(double rad)
{
	return ( (rad * 360.0) / (2.0*_pi_) );
};

/*-------------------------------*/

int roundToInt(double x)
{
	int partie_entiere = (int) x;
	double partie_deci = x - (double) partie_entiere;

	if(partie_deci >= 0.5)
		partie_entiere ++; 
	return partie_entiere; 
}

/*-------------------------------*/

double paw2(double x)
{
	return x*x;
}


int paw2(int x)
{
	return x*x;
}


std::string lastError()
{
	return _lastError_;
}


void lastError(const std::string& str_info, const std::string& str_info_2, const std::string& str_info_3)
{
	_lastError_ = str_info+str_info_2+str_info_3;
}


bool lastErrorAutomaticLog()
{
	static std::string previous_last_error;

	if(_lastError_ != previous_last_error)
	{
		std::cout << "\n________" << _lastError_ << "\n";
		previous_last_error = _lastError_;
		return true;
	}

	return false;
}


std::string str(const char* c_chaine)
{
	return std::string(c_chaine);
}



bool isInInterval_in_in(double min, double max, double val)
{
	/*on teste si : "val ∈ [min ; max]" est vrai*/

	if(min>max)
		return false;

	if(min <= val && val <= max)
		return true;
	return false;
}


bool isInInterval_in_out(double min, double max, double val)
{
	/*on teste si : "val ∈ [min ; max[" est vrai*/

	if(min>max)
		return false;

	if(min <= val && val < max)
		return true;
	return false;
}


bool isInInterval_out_in(double min, double max, double val)
{
	/*on teste si : "val ∈ ]min ; max]" est vrai*/

	if(min>max)
		return false;

	if(min < val && val <= max)
		return true;
	return false;
}


bool isInInterval_out_out(double min, double max, double val)
{
	/*on teste si : "val ∈ ]min ; max[" est vrai*/
	
	if(min>max)
		return false;

	if(min < val && val < max)
		return true;
	return false;
}




double euclideanDist(double x1, double y1, double x2, double y2)
{
	return std::sqrt(  (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)  );
}
int euclideanDist(int x1, int y1, int x2, int y2)
{
	return std::sqrt(  (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)  );
}


double pointsDist(double x1, double y1, double x2, double y2)
{
	return euclideanDist(x1, y1, x2, y2);
}
int pointsDist(int x1, int y1, int x2, int y2)
{
	return euclideanDist(x1, y1, x2, y2);
}


double pointsDist(const Vec2d& pt1, const Vec2d& pt2)
{
	return euclideanDist(pt1.x, pt1.y, pt2.x, pt2.y);
}
int pointsDist(const Vec2i& pt1, const Vec2i& pt2)
{
	return euclideanDist(pt1.x, pt1.y, pt2.x, pt2.y);
}



SDL_FRect frect(SDL_Rect rect) 
{
	return SDL_FRect(
		static_cast<float>(rect.x), static_cast<float>(rect.y), 
		static_cast<float>(rect.w), static_cast<float>(rect.h));
	
}

SDL_Rect rect(SDL_FRect rect) 
{
	return SDL_Rect(
		static_cast<int>(rect.x), static_cast<int>(rect.y), 
		static_cast<int>(rect.w), static_cast<int>(rect.h));
}



SDL_Rect rect(int x, int y, int w, int h)
{
	SDL_Rect _rect = {x, y, w, h};
	return _rect;
}


SDL_Color color(int r, int g, int b, int a)
{
	bool have_out_of_range_value = false;
	int given_r=r, given_g=g, given_b=b, given_a=a;

	if(r < 0) { r = 0; have_out_of_range_value = true; }
	if(g < 0) { g = 0; have_out_of_range_value = true; }
	if(b < 0) { b = 0; have_out_of_range_value = true; }
	if(a < 0) { a = 0; have_out_of_range_value = true; } 

	if(r > 255) { r = 255; have_out_of_range_value = true; }
	if(g > 255) { g = 255; have_out_of_range_value = true; }
	if(b > 255) { b = 255; have_out_of_range_value = true; }
	if(a > 255) { a = 255; have_out_of_range_value = true; }

	Uint8 uint8_r = static_cast<Uint8>(r);
	Uint8 uint8_g = static_cast<Uint8>(g);
	Uint8 uint8_b = static_cast<Uint8>(b);
	Uint8 uint8_a = static_cast<Uint8>(a);

	if(have_out_of_range_value)
	{
		gs::lastError(gs::str("||Error|| in 'gs::color(4 int)', have out of range value-s. Values must be in [0 ; 255]. ")
					+ gs::str("[given values : r ")+std::to_string(given_r)+gs::str(", ")
					+ gs::str("g ")+std::to_string(given_g)+gs::str(", ")
					+ gs::str("b ")+std::to_string(given_b)+gs::str(", ")
					+ gs::str("a ")+std::to_string(given_a)+gs::str("]. ")
					);
	}

	SDL_Color _color = {uint8_r, uint8_g, uint8_b, uint8_a};
	return _color;
}




SDL_Color color(float r, float g, float b, float a)
{
	bool have_out_of_range_value = false;
	float given_r=r, given_g=g, given_b=b, given_a=a;

	if(r < 0) { r = 0; have_out_of_range_value = true; }
	if(g < 0) { g = 0; have_out_of_range_value = true; }
	if(b < 0) { b = 0; have_out_of_range_value = true; }
	if(a < 0) { a = 0; have_out_of_range_value = true; } 

	if(r > 1) { r = 1; have_out_of_range_value = true; }
	if(g > 1) { g = 1; have_out_of_range_value = true; }
	if(b > 1) { b = 1; have_out_of_range_value = true; }
	if(a > 1) { a = 1; have_out_of_range_value = true; }

	Uint8 uint8_r = static_cast<Uint8>(std::round(255.0f * r));
	Uint8 uint8_g = static_cast<Uint8>(std::round(255.0f * g));
	Uint8 uint8_b = static_cast<Uint8>(std::round(255.0f * b));
	Uint8 uint8_a = static_cast<Uint8>(std::round(255.0f * a));

	if(have_out_of_range_value)
	{
		gs::lastError(gs::str("||Error|| in 'gs::color(4 float)', have out of range value-s. Values must be in [0 ; 1]. ")
					+ gs::str("[given values : r ")+std::to_string(given_r)+gs::str(", ")
					+ gs::str("g ")+std::to_string(given_g)+gs::str(", ")
					+ gs::str("b ")+std::to_string(given_b)+gs::str(", ")
					+ gs::str("a ")+std::to_string(given_a)+gs::str("]. ")
					);
	}

	SDL_Color _color = {uint8_r, uint8_g, uint8_b, uint8_a};

	return _color;
}



SDL_Color color(uint32_t ucolor) //RGBA
{
	uint8_t* ucol_ptr = (uint8_t*)(&ucolor);

	SDL_Color col = {ucol_ptr[3], ucol_ptr[2], ucol_ptr[1], ucol_ptr[0]};

	return col; 
}


SDL_Color color(SDL_FColor col) {return SDL_Color(col.r, col.g, col.b, col.a);}

SDL_FColor fcolor(SDL_Color col) {return SDL_FColor(col.r, col.g, col.b, col.a);}


SDL_FRect frect(float x, float y, float w, float h) {return SDL_FRect(x,y,w,h);}

//SDL_FColor fcolor(int r, int g, int b, int a=255); 

SDL_FColor fcolor(float r, float g, float b, float a) {return SDL_FColor(r,g,b,a);}

SDL_FColor fcolor(uint32_t ucolor) {return fcolor(color(ucolor));} //RGBA



uint32_t u32RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	uint32_t ucol;
	uint8_t* ucol_ptr = (uint8_t*)(&ucol);
	ucol_ptr[3] = r;
	ucol_ptr[2] = g;
	ucol_ptr[1] = b;
	ucol_ptr[0] = a;
	return ucol;
}


uint32_t u32RGBA(SDL_Color col)
{
	return u32RGBA(col.r, col.g, col.b, col.a);
}



uint32_t u32ABGR(uint32_t rgba)
{
	uint8_t* ptr = (uint8_t*)(&rgba);
	uint8_t r=ptr[3], g=ptr[2], b=ptr[1], a=ptr[0];

	ptr[3] = a;
	ptr[2] = b;
	ptr[1] = g;
	ptr[0] = r;

	return rgba;
}


uint32_t u32RGBA(uint32_t abgr)
{
	uint8_t* ptr = (uint8_t*)(&abgr);
	uint8_t a=ptr[3], b=ptr[2], g=ptr[1], r=ptr[0];

	ptr[3] = r;
	ptr[2] = g;
	ptr[1] = b;
	ptr[0] = a;

	return abgr;
}



uint32_t u32RGBA(SDL_FColor col) {return u32RGBA(color(col));}



short sign(double val)
{
	if(val < 0)
		return -1;
	return 1;
}

short sign(int val)
{
	if(val < 0)
		return -1;
	return 1;
}




std::string strToupper(std::string str)
{
	for(char& c : str)
		c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

	return str;
}


std::vector<std::string> strToupper(std::vector<std::string> str_vec)
{
    for(std::string& str : str_vec)
        for(char& c : str)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            
    return str_vec;
}






bool drawRect(SDL_Renderer* ren, const SDL_FRect* rect, const SDL_FColor* col, const bool& fill, SDL_BlendMode* b_mode)
{
	if(not ren)
	{
		gs::lastError("in 'drawRect()', invalid renderer. ");
		return false;
	}

	SDL_Color previous_c;
	if(col)
	{
		SDL_GetRenderDrawColor(ren, &previous_c.r, &previous_c.g, &previous_c.b, &previous_c.a);
		SDL_SetRenderDrawColor(ren, col->r, col->g, col->b, col->a);
	}
	
	SDL_BlendMode previous_blend_mode;
	if(b_mode)
	{
		SDL_GetRenderDrawBlendMode(ren, &previous_blend_mode);
		SDL_SetRenderDrawBlendMode(ren, *b_mode);
	}

	bool res;
		
	if(fill)
		res = SDL_RenderFillRect(ren, rect);
	else
		res = SDL_RenderRect(ren, rect);
	
	if(b_mode)
		SDL_SetRenderDrawBlendMode(ren, previous_blend_mode);
	
	if(col)
		SDL_SetRenderDrawColor(ren, previous_c.r, previous_c.g, previous_c.b, previous_c.a);

	if(!res)
	{
		gs::lastError(gs::str("in 'drawRect()' : ")+SDL_GetError());
		return false;
	}

	return true;
}

bool drawRect(SDL_Renderer* ren, const SDL_FRect& rect, const SDL_FColor* col, const bool& fill, SDL_BlendMode* b_mode)
{
	return drawRect(ren, &rect, col, fill, b_mode);
}




bool drawRect(SDL_Renderer* ren, const SDL_FRect* rect, const SDL_FColor& col, const bool& fill, SDL_BlendMode* b_mode)
{
	return drawRect(ren, rect, &col, fill, b_mode);
}

bool drawRect(SDL_Renderer* ren, const SDL_FRect& rect, const SDL_FColor& col, const bool& fill, SDL_BlendMode* b_mode)
{
	return drawRect(ren, &rect, &col, fill, b_mode);
}






bool drawLine(SDL_Renderer* ren, const Vec2f& pt1, const Vec2f& pt2, const SDL_FColor& col, SDL_BlendMode* b_mode)
{
	if(not ren)
	{
		gs::lastError("in 'drawLine()', invalid renderer. ");
		return false;
	}

	SDL_Color previous_c;
	SDL_GetRenderDrawColor(ren, &previous_c.r, &previous_c.g, &previous_c.b, &previous_c.a);
	SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.a);

	SDL_BlendMode previous_blend_mode;

	if(b_mode)
	{
		SDL_GetRenderDrawBlendMode(ren, &previous_blend_mode);
		SDL_SetRenderDrawBlendMode(ren, *b_mode);
	}

	bool res = SDL_RenderLine(ren, pt1.x, pt1.y, pt2.x, pt2.y);

	if(b_mode)
		SDL_SetRenderDrawBlendMode(ren, previous_blend_mode);
	
	SDL_SetRenderDrawColor(ren, previous_c.r, previous_c.g, previous_c.b, previous_c.a);

	if(!res)
	{
		gs::lastError(std::string("In 'drawLine()' : ")+SDL_GetError()); 
		return false;
	}

	return true;
}


//bool drawLine(SDL_Renderer* ren, const Vec2i& pt1, const Vec2i& pt2, const SDL_FColor& col, SDL_BlendMode* b_mode)
//{
//	return drawLine(ren, pt1.to<float>(), pt2.to<float>(), col, b_mode);
//}





double dt()
{
	if(ThisGame) return ThisGame->dt();
	return 0.0;
}



SDL_Renderer* ren()
{
	if(ThisGame) return ThisGame->ren();
	return nullptr;
}

SDL_Window* win()
{
	if(ThisGame) return ThisGame->win();
	return nullptr;
}







double getAngle(double origin_x, double origin_y, double point_x, double point_y, bool clockwise)
{
	//Retourne l'angle entre le point donné (P), l'origine donnée (O), et un point (Z) quelconque sur l'axe des abscisses à droite de l'origine. 
	//
	//-------O--------Z-------
	//
	//          P
	//         _
	// renvoi POZ.

	if(origin_y == point_y)
	{
		if(origin_x <= point_x) //Angle nul.
			return 0;
		return M_PI; //Angle plat.
	} 

	gs::Vec2d vector = {point_x-origin_x, point_y-origin_y};
	
	//==========================================================//
		// Formule : 
		// << cah soh toa >>
		//
		// tan(angle) == opposé / adjacent
		// angle      == arctan(opposé / adjacent)
		//
		// adjacent == vector.x
		// opposé == vector.y
		//
		//           
		//         _vy
		//       _v  y
		//     _v    y
		//   _v      y
		// _v a      y
		// oxxxxxxxxxy
		//
		// o : origine
		// a : angle 
		// v : vecteur
		// x : adjacent
		// y : opposé
		//
	//==========================================================//

	//En radian
	double angle = std::atan(vector.y/vector.x);
	
	//Ajustement puisque le triangle rectangle formé n'a pas toujours la même position.   
	if(point_x < origin_x)
		angle += M_PI; //En ce qui concerne '+= pi', je l'ai trouvé à tâtons.

	//Facultatif. Afin d'avoir un angle toujours sur [0; 2pi].
	if(angle < 0)
		angle = 2*M_PI + angle; // '+' car 'angle' est négatif. 

	if(not clockwise)
		angle = 2*M_PI - angle;

	return angle;
}



double getAngle(const Vec2d& origin, const Vec2d& point, bool clockwise)
{
	return getAngle(origin.x, origin.y, point.x, point.y, clockwise);
}




gs::Vec2d rotatePoint(double center_x, double center_y, double point_x, double point_y, double angle_rad, bool clockwise)
{
	float sign = (clockwise ? 1 : -1);
	
	double new_angle = getAngle(center_x, center_y, point_x, point_y, true) + angle_rad*sign;

	double dist = gs::euclideanDist(center_x, center_y, point_x, point_y);

	gs::Vec2d vector;
	vector.x = std::cos(new_angle) * dist;
	vector.y = std::sin(new_angle) * dist;

	gs::Vec2d center; 
	center.x = center_x;
	center.y = center_y;

	return center + vector;
}




bool drawPoint(SDL_Renderer* ren, const int& x, const int& y, const SDL_Color* col, SDL_BlendMode* b_mode)
{
	if(not ren)
	{
		gs::lastError("In 'drawPoint()', invalid renderer. ");
		return false;
	}

	SDL_Color previous_c;
	if(col)
	{
		SDL_GetRenderDrawColor(ren, &previous_c.r, &previous_c.g, &previous_c.b, &previous_c.a);
		SDL_SetRenderDrawColor(ren, col->r, col->g, col->b, col->a);
	}
	
	SDL_BlendMode previous_blend_mode;
	if(b_mode)
	{
		SDL_GetRenderDrawBlendMode(ren, &previous_blend_mode);
		SDL_SetRenderDrawBlendMode(ren, *b_mode);
	}

	bool res = SDL_RenderPoint(ren, x, y);

	if(b_mode)
		SDL_SetRenderDrawBlendMode(ren, previous_blend_mode);
	
	if(col)
		SDL_SetRenderDrawColor(ren, previous_c.r, previous_c.g, previous_c.b, previous_c.a);

	if(!res)
	{
		gs::lastError(gs::str("In 'drawPoint()' : ")+SDL_GetError());
		return false;
	}

	return true;
}

bool drawPoint(SDL_Renderer* ren, const int& x, const int& y, const SDL_Color& col, SDL_BlendMode* b_mode)
{
	return drawPoint(ren, x, y, &col, b_mode);
}


bool drawPoint(SDL_Renderer* ren, const Vec2i& pt, const SDL_Color* col, SDL_BlendMode* b_mode)
{
	return drawPoint(ren, pt.x, pt.y, col, b_mode);
}

bool drawPoint(SDL_Renderer* ren, const Vec2i& pt, const SDL_Color& col, SDL_BlendMode* b_mode)
{
	return drawPoint(ren, pt.x, pt.y, &col, b_mode);
}





uint8_t alphaByPercentage(float percent)
{
	float res = 255.0*percent;
	if(res < 0) res = 0;
	if(res > 255) res = 255;

	return uint8_t(res);
}


Vec2d orthoVecOf(const Vec2d& vector_)
{
	return Vec2d(-vector_.y, vector_.x);
}

Vec2i orthoVecOf(const Vec2i& vector_)
{
	return Vec2i(-vector_.y, vector_.x);
}

Vec2i orthoVecOf_i(const Vec2d& vector_)
{
	return orthoVecOf(vector_).to<int>();
}

Vec2d orthoVecOf_d(const Vec2i& vector_)
{
	return orthoVecOf(vector_).to<double>();
}





Vec2d normalizedVecOf(const Vec2d& vector_)
{
	double len = pointsDist(Vec2d(0,0), vector_);
	return Vec2d(vector_.x/len, vector_.y/len);
}

Vec2d normalizedVecOf(const Vec2i& vector_)
{
	return normalizedVecOf(vector_.to<double>());
}





bool lineAndCircleIntersect(const double& line_a, const double& line_b, const double& line_c, 
	const Vec2d& cir_center, const double& cir_radius, Vec2d* out__pt1, Vec2d* out__pt2)
{
	//Intersection entre droite et cercle.
	const double  &a=line_a,  &b=line_b,  &c=line_c, &m=cir_center.x,  &n=cir_center.y,  &r=cir_radius;

	double A = a*a + b*b;
	double B = (-2)*b*b*m + 2*a*(c+b*n);
	double C = b*b*m*m + (c+b*n)*(c+b*n) - b*b*r*r;

	if(A!=0)
	{
		double x1 = ((-B)-std::sqrt(B*B - 4*A*C))/(2*A);
		double x2 = ((-B)+std::sqrt(B*B - 4*A*C))/(2*A);
	
		if(b != 0)
		{
			if(out__pt1)
			{
				out__pt1->x = x1;
				out__pt1->y = (-a*x1-c)/b;
			}
			
			if(out__pt2)
			{
				out__pt2->x = x2;
				out__pt2->y = (-a*x2-c)/b;
			}

			return true;
		}
	}

	return false;
}

bool lineAndCircleIntersect_i(const double& line_a, const double& line_b, const double& line_c, 
	const Vec2d& cir_center, const double& cir_radius, Vec2i* out__pt1, Vec2i* out__pt2)
{
	Vec2d out1, out2;
	bool res = lineAndCircleIntersect(line_a, line_b, line_c, cir_center, cir_radius, &out1, &out2);

	if(out__pt1)
		(*out__pt1) = out1.to<int>();
	if(out__pt2)
		(*out__pt2) = out2.to<int>();

	return res;
}





Vec2d orthoProj(const Vec2d& point, const double& line_a, const double& line_b, const double& line_c)
{
	//Projeté orthogonal du point (m,n) sur la droit d'équation ax + by + c.
	const double  &a=line_a,  &b=line_b,  &c=line_c,  &m=point.x,  &n=point.y;
	
	Vec2d orth_proj;

	if((-a*a-b*b) != 0 and b != 0)
	{
		orth_proj.x = (c*a +b*(-b*m +a*n))/(-a*a-b*b);
		orth_proj.y = (-a*((c*a +b*(-b*m +a*n))/(-a*a-b*b))-c)/b;
	}

	return orth_proj;
}

Vec2i orthoProj_i(const Vec2i& point, const double& line_a, const double& line_b, const double& line_c)
{
	return orthoProj(point.to<double>(), line_a, line_b, line_c).to<int>();
}







bool isInField(const gs::Vec2d& field_origin, const double& fov, const double& dir_rad, const double& field_radius,
				const gs::Vec2d& circ_origin, const double& circ_radius, 
				gs::Vec2d* out__closest_pt, double* out__origin_proximity)
{
	//Cette fonction vérifie la présence d'un disque/cercle (entité) dans un morceau de disque (champ de vision, zone d'action ...).
	//Le morceau de disque (ou disque complet) est déterminé par : une position d'origine (centrale), un angle (l'ouverture -> fov), une direction centrale associée à un sens (un angle), et un étendu (rayon du champ).
	//Ce morceau peut être assimilé à une part de gateau. 

	bool check_extra = (out__closest_pt!=nullptr) or (out__origin_proximity!=nullptr);
	gs::Vec2d closest_pt;
	gs::Vec2d pt1 = circ_origin, pt2=circ_origin;
	bool is_in_field = false;

	////Vérification de la présence du cercle dans le rayon du field.
	////
	////
	int centers_dist = gs::pointsDist(field_origin, circ_origin);
	
	//Cas particulier: si l'origine du champ est dans le cercle (notamment [mais pas forcément] si le cercle est plus grand que le champ.
	if(centers_dist < circ_radius)
	{
		if(check_extra)
			closest_pt = field_origin; 

		is_in_field = true;
	}
	else
	if(centers_dist < (field_radius+circ_radius)) //Si les deux sont dans le rayon de l'un et l'autre.
	{
		////Important : ici le cercle est dans le rayon du field.
		////Vérification de la présence du centre du cercle dans l'angle de vu.
		////
		////
		double dir = dir_rad;
		if(not gs::isInInterval_in_in(0, 2.0*_pi_, dir))
		{
			//Cette projection permet de déterminer la direction du champ formaté dans l'interval [0; 2pi].
			gs::Vec2d projection_pt = {field_origin.x +std::cos(dir)*10.0,
									field_origin.y +std::sin(dir)*10.0};
			
			dir = gs::getAngle(field_origin.x, field_origin.y, projection_pt.x, projection_pt.y, true);
		}

		float angle_with_circ = gs::getAngle(field_origin.x, field_origin.y, circ_origin.x, circ_origin.y, true);

		float min_dir=dir-fov/2.0; //Direction de la bordure minimale du field.
		float max_dir=dir+fov/2.0; //Direction de la bordure maximale du field.
		if(min_dir<0) //Les réglages suivants, nécessaires au bon fonctionnement de l'implémentation, ont été trouvés de manière expérimentale, empiriquement.
		{
			min_dir+=2.0*_pi_; //On les rajoute un tour, pour éviter les valeurs négatives.
			max_dir+=2.0*_pi_;

			if(angle_with_circ < _pi_) //Si l'angle est dans la première moité du cercle trigonométrique.
				angle_with_circ += 2.0*_pi_; //On le trourne aussi afin qu'il soit au "même tour" que min_dir (pour avoir un résultat correct).
		}
		else
		if(angle_with_circ < _pi_ and angle_with_circ < min_dir)
			angle_with_circ += 2.0*_pi_; //On le trourne afin qu'il soit au "même tour" que min_dir (pour avoir un résultat correct).

		if(gs::isInInterval_in_in(min_dir, max_dir, angle_with_circ)) //Enfin, si le centre du cercle est dans l'angle du field.
		{
			if(check_extra)
			{
				closest_pt.x = field_origin.x+std::cos(angle_with_circ)*(centers_dist-circ_radius);
				closest_pt.y = field_origin.y+std::sin(angle_with_circ)*(centers_dist-circ_radius);
			}

			is_in_field = true; //Ainsi si le cercle est (partiellement ou totalement) dans le rayon du field et est dans l'angle du field, alors le cercle est dans le field (partiellement ou totalement).
		}
		else
		{
			//Sinon, il se peut que malgré le fait que le centre ne soit pas dans l'angle du field, un partie du cercle soit tout de même dans le field.
			//Alors on vérifie l'intersection entre le cercle et les bordures du field.
		
			gs::Line_Segment field_min_edge(field_origin, {field_origin.x+std::cos(min_dir)*field_radius, field_origin.y+std::sin(min_dir)*field_radius});
			gs::Line_Segment field_max_edge(field_origin, {field_origin.x+std::cos(max_dir)*field_radius, field_origin.y+std::sin(max_dir)*field_radius});

			gs::Circle circle(circ_origin, circ_radius);
			bool edge = false;

			//Teste si le cercle touche la bordure minimale du champ.
			gs::Vec2b res = field_min_edge.intersect(circle, &pt1, &pt2);
			if(res.x or res.y)
			{
				edge = true;
			}
			else
			{
				//Teste si le cercle touche la bordure maximale du champ.
				res = field_max_edge.intersect(circle, &pt1, &pt2);
				if(res.x or res.y)
					edge = true;
			}

			if(edge and check_extra)
			{
				double dist_pt1 = gs::pointsDist(field_origin, pt1);
				double dist_pt2 = gs::pointsDist(field_origin, pt2);
				
				if(dist_pt2 < dist_pt1)
					closest_pt = pt2;
				else
					closest_pt = pt1;
			}

			is_in_field = edge;
		}

		
	}
	
	//Si le cercle n'est pas au moins dans le rayon du field, alors il ne s'y trouve pas.

	if(is_in_field and check_extra)
	{
		if(out__closest_pt)
			(*out__closest_pt) = closest_pt;

		if(out__origin_proximity)
		{
			if(field_radius==0)
				(*out__origin_proximity) = 1.0;
			else
				(*out__origin_proximity) = 1.0-gs::pointsDist(field_origin, closest_pt)/field_radius;
		}
	}

	return is_in_field; 
}







double decimalPart(double val)
{
	double int_part = double(int(val));
	return val - int_part;
}

float decimalPart(float val)
{
	float int_part = float(int(val));
	return val - int_part;
}








float perlinNoise2D(float x, float y, unsigned int seed)
{
	//Pour l'instant la seed est inutilisée. 

	auto getNodeVec = [&seed] (int ix, int iy)
	{
		// float s = seed, x=ix, y=iy;
		float angle = (ix*ix + iy*12.0 - iy*iy + 20*ix);  
		// float angle = (x*x*x-y + y*(s/12.0) - y*y*y+x*x + (s/20.0)*x);  
		// float angle = std::cos(y) + std::sin(x*x*x)*2.0 +s/40.0;
		
	    //Renvoie du vecteur.
	    return gs::Vec2f(std::cos(angle), std::sin(angle));
	};



	auto interpol = [](float a, float b, float w) {
		return (b - a) * (3.0 - w * 2.0) * w * w + a;
	};

	
	//On suppose qu'une cellule a une taille de 1.
	float node_x = std::floor(x);
	float node_y = std::floor(y);
	float node_nx = node_x+1;
	float node_ny = node_y+1;

	//Distances.
	float dx = x - node_x;
	float dy = y - node_y;

	//Vecteurs de la grille.
	gs::Vec2f tl = getNodeVec(node_x,node_y);
	gs::Vec2f tr = getNodeVec(node_nx,node_y);
	gs::Vec2f bl = getNodeVec(node_x,node_ny);
	gs::Vec2f br = getNodeVec(node_nx,node_ny);

	//Les vecteurs distance. 
	gs::Vec2f tl2(x-node_x, y-node_y); 
	gs::Vec2f tr2(x-node_nx, y-node_y);  
	gs::Vec2f bl2(x-node_x, y-node_ny);   
	gs::Vec2f br2(x-node_nx, y-node_ny); 
	
	SDL_FRect vals;
	vals.x = gs::dotProd(tl,tl2);
	vals.y = gs::dotProd(tr,tr2);
	vals.w = gs::dotProd(bl,bl2);
	vals.h = gs::dotProd(br,br2);


	float interp1 = interpol(vals.x, vals.y, dx);
	float interp2 = interpol(vals.w, vals.h, dx);

	return interpol(interp1, interp2, dy);
}




// Algorithme de Bresenham pour tracer une ligne entre deux points
std::vector<Vec2i> bresenham(int x0, int y0, int x1, int y1) 
{
	//Code généré par I.A, et très légèrement adapté.

    std::vector<Vec2i> points; // Vecteur pour stocker les points de la ligne
    int dx = abs(x1 - x0);         // Différence absolue entre les coordonnées x
    int dy = abs(y1 - y0);         // Différence absolue entre les coordonnées y
    int sx = (x0 < x1) ? 1 : -1;   // Direction du pas pour x (1 ou -1)
    int sy = (y0 < y1) ? 1 : -1;   // Direction du pas pour y (1 ou -1)
    int err = dx - dy;             // Calcul de l'erreur initiale

    while (true) { // Boucle qui continue jusqu'à atteindre le point final
        points.push_back({x0, y0}); // Ajoute le point courant à la liste des points
        if (x0 == x1 && y0 == y1) { // Vérifie si le point final est atteint
            break;                  // Sort de la boucle
        }
        int e2 = 2 * err;           // Double l'erreur pour comparer avec dx et dy
        if (e2 > -dy) {             // Vérifie si l'on doit avancer en x
            err -= dy;              // Réajuste l'erreur en retirant dy
            x0 += sx;               // Avance en x dans la direction correcte
        }
        if (e2 < dx) {              // Vérifie si l'on doit avancer en y
            err += dx;              // Réajuste l'erreur en ajoutant dx
            y0 += sy;               // Avance en y dans la direction correcte
        }
    }
    return points; // Renvoie la liste des points de la ligne
}








unsigned int countCharsUTF8_ByCharHeader(const char& ch, bool& is_ok)
{
	is_ok=true;
	
	try{
		
	
	std::string byte = std::bitset<8>(ch).to_string();
	
	if(byte.at(0) == '0') //C'est un caractère composé d'un seul octet.
	{
		return 1u;
	}
	else //Sinon c'est que 'byte.at(0)' c'est (normalement) un '1'.
	{
		if(byte.at(1) == '1' and byte.at(2) == '0') //Suite de 'char's composant un caractère utf-8 de deux octets.
		{
			return 2u;
		}
		else
		if(byte.at(1) == '1' and byte.at(2) == '1' and byte.at(3) == '0') //Suite de 'char's composant un caractère utf-8 de trois octets.
		{
			return 3u;
		}
		else
		if(byte.at(1) == '1' and byte.at(2) == '1' and byte.at(3) == '1' and byte.at(4) == '0') //Suite de 'char's composant un caractère utf-8 de quatre octets.
		{
			return 4u;
		}
	}
	
	
	}catch(std::exception& err)
	{
		std::cout << "!! in 'countCharsUTF8_ByCharHeader()' : " << err.what() << '\n';
		is_ok = false;
	}
	
	return 0; //Si on arrive ici, et que 'is_ok==true', c'est (probablement) que l'octet reçu est un octet compasant un caractère tout en étant pas le premier octet de ce caractère.
}


unsigned int countCharsUTF8_ByCharHeader(const char& ch)
{
	bool skip_arg;
	return countCharsUTF8_ByCharHeader(ch, skip_arg);
}



unsigned int countCharsUTF8(const std::string& str, bool& is_ok)
{
	unsigned int utf8_chars_count = 0;
	unsigned int skip = 0;
	is_ok = true;
	
	
	for(const char& ch : str)
	{
		if(skip>0)
		{
			--skip;
			continue;
		}
		
		unsigned int curr_char_chars_count = countCharsUTF8_ByCharHeader(ch, is_ok);
		if(not is_ok or curr_char_chars_count==0)
			return 0;
		
		++utf8_chars_count;
		skip = curr_char_chars_count-1;
	}
	
	return utf8_chars_count;
}


unsigned int countCharsUTF8(const std::string& str)
{
	bool skip_arg;
	return countCharsUTF8(str, skip_arg);
}




bool isFlagPresentIn(int combined_flags, int specifiq_flag)
{
	return (combined_flags & specifiq_flag) == specifiq_flag;
}











































//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//________________________FONCTIONS  DU MIXER (MUSIQUES)______________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________


/*
	bool Game::initMixer(int frequency, Uint16 format, int channels, int chunksize)
	{

		if(initMix_is_allowed)
		{
			if(Mix_OpenAudio( frequency, format, channels, chunksize) == 0)
			{
				mixActive_ = true;
				initMix_is_allowed = false;
				allocated_channels += channels;
				return true;
			}
			
			std::cout << "\t|| Erreur || `initMixer()' failled : "<< Mix_GetError() << std::endl;
			mixActive_ = false;
			return false;
		}
		else
		{
			std::cout << "\t|| Warning || `initMixer()' already used. Close the Mixer befor trying " << std::endl;
			return false;
		}
	}

//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//


	void Game::closeMixer()
	{
		if(mixActive_)
		{
			Mix_CloseAudio();
			mixActive_ = false;
			initMix_is_allowed = true;
		}
	}


//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//


int  Game::getFree_channel() 
{
	if(allocated_channels == free_channel_)						//allocated_channels est le nombre de channel alloué, free_channel est le numero du plus petit channel libre mais aussi le nb de channel occupés 
	{
		allocated_channels = Mix_AllocateChannels(allocated_channels+1);
	}	

	free_channel_+= 1; 

	return free_channel_-1;
};
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//


Music::Music()
{music = nullptr; volume = 50; };

//------------------------------------------------------------

Music::~Music(){ Mix_FreeChunk(music); }

//------------------------------------------------------------

bool Music::load(Game *g, const char *file)
{
	if(music != nullptr)
		Mix_FreeChunk(music); 
	music = nullptr;

	music = Mix_LoadWAV(file); selfChannel = g->getFree_channel();
	
	if(music != nullptr) 
		return true; 
	return false;
};

//------------------------------------------------------------

void Music::play(int boucle)
{
	if(!Mix_Playing(selfChannel))
		Mix_PlayChannel(selfChannel, music, boucle);
}

//------------------------------------------------------------

void Music::stop(){ Mix_HaltChannel(selfChannel); };

//------------------------------------------------------------

void Music::pause(){ Mix_Pause(selfChannel); };

//------------------------------------------------------------

void Music::resume(){ Mix_Resume(selfChannel); };

//------------------------------------------------------------

void Music::setVolume(int vol)
{
	volume = vol;
	Mix_Volume(selfChannel, volume);
}

//------------------------------------------------------------

void Music::incrVolume(int val)
{
	volume += val;
	Mix_Volume(selfChannel, volume);
}



*/




















//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//_________________________FONCTIONS  DES  PARTICULES (old)___________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________


		bool ParticleBox::is_empty()
		{
			if(box.size()<=0)
				return true;
			return false;
		}

//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//

		void ParticleBox::add_particle(std::string typ,
		int xmin,int xmax,    int ymin,int ymax,    float vxmin,float vxmax, 
		float vymin,float vymax,        float gmin, float gmax,
							  float lifeT)
		{

			 std::uniform_int_distribution<int>    _iks{xmin, xmax}, _igrec{ymin, ymax};
			 std::uniform_real_distribution<float> _viks{vxmin, vxmax}, _vigrec{vymin, vymax},
			  									  _gravity{gmin, gmax};

			igrec.param(_igrec.param()); iks.param(_iks.param());
			vigrec.param(_vigrec.param()); viks.param(_viks.param());
			gravity.param(_gravity.param());



			//creation de la particule
			pParticle p;
			
			//son initialisation 
			p.x = iks(re);      p.vx = viks(re) ;      p.gravity = gravity(re);  p.next = nullptr;
		    p.y = igrec(re);    p.vy = vigrec(re);   p.type = typ;  p.lifeTime = lifeT;         
			 
			//son ajout dans la liste
			box.push_back(p);
			nb++;

		}
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//

		void ParticleBox::remove_back()
		{
			box.pop_back(); nb--;
		}

//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//

		void ParticleBox::update()
		{
			//if(!is_empty())
			{

				for(int i{0};  i< box.size();  i++)
				{
					pParticle& current = box[i];
					//if(current.lifeTime < 0)  pas encore pret
					/* */
					current.tmpvx += current.vx;
							    if(current.tmpvx >= 1 || current.tmpvx <= -1)
							    {
							        int partieEntiere = (int) current.tmpvx;  
							        //incrementation de la valeur valable 
							        current.x += partieEntiere;  
							        //retrait de la partie utilisée
							        current.tmpvx  -= (float) partieEntiere;
							    }

					current.tmpvy += current.vy;
							    if(current.tmpvy >= 1 || current.tmpvy <= -1)
							    {
							        int partieEntiere = (int) current.tmpvy;  
							        //incrementation de la valeur valable 
							        current.y += partieEntiere;  
							        //retrait de la partie utilisée
							        current.tmpvy  -= (float) partieEntiere;
							    }
					current.vy += current.gravity;

					/* */
					

					
				}

			}	


		}

//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//


		void ParticleBox::draw()
		{	
			if(game != nullptr)
			{	
				if(!is_empty())
				{
					

					for(int i{0};  i< box.size(); i++)
					{
						pParticle current = box[i];
							SDL_RenderPoint(game->ren(), current.x, current.y);
							
								//SDL_Rect rec = {current.x, current.y, 4,4};
								//SDL_Rect rec1 = {50,50,50,50};
								//if(SDL_RenderFillRect(game->ren(), &rec) != 0)
								//	std::cout<<"\n"<<SDL_GetError();
								//if(SDL_RenderCopy(ThisGame->ren(), tex, NULL, &rec) != 0)
								//	std::cout<<"\n"<<SDL_GetError();
								//std::cout<<i;
							
							
							//std::cout<<std::endl; 
					}
				}
			}
			else 
			{
				lastError(std::string("|| Warning || in 'gs::ParticleBox::draw()', trying to draw particles with a `ParticleBox' witch doesn't have an affected `gs::Game' object"));
			}
		}

//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//

		void ParticleBox::clear()
		{
			if(is_empty())
				return;
			nb = 0;
			box.clear();
			// if(nb != 0)
			// {
			// 	std::cout<< "\n\t|| Warning || after `ParticleBox::clear()' nb != 0  : "<<nb<<std::endl;
			// }
			std::cout<<"\tparticles clear"<<std::endl;
		}









































} //fin namespace gs
