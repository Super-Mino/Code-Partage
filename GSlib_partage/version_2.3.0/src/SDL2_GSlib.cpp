#include "SDL2_GSlib.hpp"
#include <random>

//version modifié le 29/06/23 (à moins d'oubli de modifier cette ligne)

sm::Game *ThisGame = nullptr; 


std::vector<SDL_Texture*> textureToClearAtTheEnd{};
std::vector<SDL_Renderer*> rendererToClearAtTheEnd{}; 
std::vector<SDL_PixelFormat*> pixelFormatToClearAtTheEnd{};
std::vector<std::vector<char>> paths_loaded_with_smloadImg{};

namespace sm
{

bool game_started = false;
bool game_won = false;
bool end_game = false;
bool show_comments = false;
std::string _lastError_;

void initFreeResources()
{
	std::atexit(freeGivenResources);
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
: mixActive_(false), initMix_is_allowed(true),
free_channel_(0), allocated_channels(0),
lastTime(SDL_GetTicks()), dt_(0)
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

	
	if(mixActive_) 
		Mix_CloseAudio();


	if(win_icon != nullptr)
		SDL_FreeSurface(win_icon);

	if(is_main_game)
		SDL_Quit();
	else
		std::atexit(SDL_Quit);

	if(show_comments)
	std::cout<<"\tEnd Game succes" << std::endl;
};



//____________________________________________________________________________________
//____________________________________________________________________________________



void Game::InitSDLEngine(int window_w, int window_h, const char* window_title, int windowpos_x, int windowpos_y, Uint32 win_flags, Uint32 ren_flags)
{
	if(isInit) return;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "\t|| Erreur || initialisation de la SDL : " << SDL_GetError() << std::endl;
		// SDL_Quit();
		exit(-1);
	};

	win_ = SDL_CreateWindow(window_title, windowpos_x, windowpos_y,
						   window_w, window_h, win_flags); 
	if(win_ == nullptr)
	{
		std::cout << "\t|| Erreur || initialisation de la fenetre : " << SDL_GetError() << std::endl;
		// SDL_Quit();
		exit(-1);
	};

	ren_ = SDL_CreateRenderer(win_, -1, ren_flags);
	if(ren_ == nullptr)
	{
		std::cout << "\t|| Erreur || initialisation du renderer : " << SDL_GetError() << std::endl;
		// SDL_Quit();
		exit(-1);
	};

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
	SDL_GetRendererOutputSize(ren(), &m_ren_w, &m_ren_h);
	return m_ren_w;
};
//----------------------------------------------
int Game::getRenH() 
{
	SDL_GetRendererOutputSize(ren(), &m_ren_w, &m_ren_h);
	return m_ren_h;
};
//----------------------------------------------
bool Game::mixActive() const {return mixActive_;};
//----------------------------------------------
std::string Game::lastError() const { return lastError();};

float Game::dt() const {return dt_;};


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



SDL_Texture *Game::loadImg(const char *pth)
{
	SDL_Texture* res = ::sm::loadImg(ren_, pth);
	if(res == nullptr)
	{
		sm::lastError(sm::str("from 'sm::Game::loadImg' : ") +sm::lastError());	
	}
	return res;
};		



//____________________________________________________________________________________
//____________________________________________________________________________________



bool Game::drawTile(SDL_Texture *tileset, int x, int y, int destB, int srcX, int srcY, int srcB, bool isRenderTheTarget)
{
	return sm::drawTile(ren_, nullptr, tileset, x, y, destB, srcX, srcY, srcB, isRenderTheTarget);
};



//____________________________________________________________________________________
//____________________________________________________________________________________



void Game::delay(unsigned int &frameLimit, int fps) const
{
    // Gestion des fps (images/seconde)
    int ticks_d_attente = 1000/fps;
    unsigned int ticks = SDL_GetTicks();
 
    if (frameLimit < ticks)
    {
        return;
    }
 
    if (frameLimit > ticks + ticks_d_attente)
    {
        SDL_Delay(ticks_d_attente);
    }
 
    else
    {
        SDL_Delay(frameLimit - ticks);
    }

    frameLimit = SDL_GetTicks()+16;
}



//____________________________________________________________________________________
//____________________________________________________________________________________



void Game::deltaTime()
{
	float nawTime = SDL_GetTicks();
	dt_ = (nawTime - lastTime)*0.001; // conversion en secondes
	lastTime = SDL_GetTicks();
}


//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::addToClearAtTheEnd(SDL_Texture* tex)
{
	::sm::addToClearAtTheEnd(tex);
}

//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::addToClearAtTheEnd(SDL_Renderer* renderer)
{
	::sm::addToClearAtTheEnd(renderer);
}

//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::addToClearAtTheEnd(SDL_PixelFormat* fmt)
{
	::sm::addToClearAtTheEnd(fmt);
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
	if(ev.type == SDL_QUIT)
		return true;

	if(ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_CLOSE)
		if(getWinID() == ev.window.windowID)
			return true;

	return false;
}


//____________________________________________________________________________________
//____________________________________________________________________________________


void Game::setWinIcon(const char* path)
{
	if(not isInit)
		return;

	if(win_icon != nullptr)
		SDL_FreeSurface(win_icon);

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





















//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//___________________________________UTILS____________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________


SDL_Texture* loadImg(SDL_Renderer *renderer, const char* path)
{
	SDL_Texture *texture = nullptr;
    texture = IMG_LoadTexture(renderer, path);
   
   
    if(not texture)
    {
    	lastError(std::string("||Error|| in 'sm::loadImg()' : cannot load the image from '") 
    		+ std::string(path) 
    		+ std::string("', : ") 
    		+ std::string(SDL_GetError()));
   
    	return nullptr;
    }
    else 	//Mise en place dans la pile de nettoyage + établissement de la provenance en tant qu'info.
    {
    	{
    		paths_loaded_with_smloadImg.emplace_back();
	    	std::vector<char>& vec_str = paths_loaded_with_smloadImg.back();

	    	std::string for_size = path;
	    	vec_str.resize(for_size.size()+1);
	    	vec_str.back() = '\0';

	    	for(int i=0; i< vec_str.size(); i++)
	    	{
	    		char c = (char)path[i];
	    		vec_str.at(i) = c;
	    	}
	    	
	    	const char* source_file_as_userdata = paths_loaded_with_smloadImg.back().data();
	        
	        SDL_SetTextureUserData(texture, (void*)source_file_as_userdata);    
		}

		::sm::addToClearAtTheEnd(texture);
    }

	return texture;
}



//____________________________________________________________________________________
//____________________________________________________________________________________



SDL_Texture *loadImgAndDeleteAColor(SDL_Renderer *renderer, const char *p, Color C)
{
	SDL_Surface *loadedImage = nullptr; SDL_Texture *texture = nullptr;
    loadedImage = IMG_Load(p);

    SDL_SetColorKey(loadedImage, SDL_TRUE, SDL_MapRGB(loadedImage->format, C.r, C.g, C.b));

    if (loadedImage != nullptr)
    {
        texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
        SDL_FreeSurface(loadedImage);
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
					int x, int y, int destB, int srcX, int srcY, int srcB, 
					bool takeInToAccountTheTarget)
{
	SDL_Rect src, dest;
	bool success = true;

	src.x = srcX;		src.y = srcY;		src.w = srcB;		src.h = srcB;

	dest.x = x;		    dest.y = y;		    dest.w = destB;		dest.h = destB; 


	SDL_Texture* previousTarget = SDL_GetRenderTarget(ren);

	if(takeInToAccountTheTarget)
		SDL_SetRenderTarget(ren, target);

	if(  SDL_RenderCopy(ren, tileset, &src, &dest) != 0  )
	{
		lastError(std::string("||Warnig|| in `sm::drawTile()' , impossible to draw the tile to [") 
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
	for(SDL_Texture* t : textureToClearAtTheEnd)
		if(t == tex)
			return;
	if(tex)
	textureToClearAtTheEnd.push_back(tex);
}


void addToClearAtTheEnd(SDL_Renderer* ren)
{
	for(SDL_Renderer* r : rendererToClearAtTheEnd)
		if(r == ren)
			return;
	if(ren)
	rendererToClearAtTheEnd.push_back(ren);
}


void addToClearAtTheEnd(SDL_PixelFormat* fmt)
{
	for(SDL_PixelFormat* f : pixelFormatToClearAtTheEnd)
		if(f == fmt)
			return;
	if(fmt)	
	pixelFormatToClearAtTheEnd.push_back(fmt);
}


void freeGivenResources()
{
	if(show_comments)
	std::cout << "\n|______Free ress______|\n";

	for(SDL_Texture* tex : textureToClearAtTheEnd)
	{
		if(tex)
		{
			if(show_comments)
			{
				std::string name;
				if((char*)SDL_GetTextureUserData(tex))
					name = (char*)SDL_GetTextureUserData(tex);
				else
					name = "(cannot get title)";
	
				std::printf("- texture : info -> %s, addr -> %p\n", name.c_str(), tex);
			}
			
			SDL_DestroyTexture(tex);
		}
		/*else std::printf("*\n");*/
	}
	textureToClearAtTheEnd.clear();
	textureToClearAtTheEnd.shrink_to_fit();

	
	for(SDL_Renderer* renderer : rendererToClearAtTheEnd)
	{
		if(renderer)
		{
			if(show_comments)
			{
				SDL_RendererInfo info;
				SDL_GetRendererInfo(renderer, &info);
				std::printf("- renderer : info -> %s, addr -> %p\n", info.name, renderer);
			}
			
			SDL_DestroyRenderer(renderer);
		}
	}
	rendererToClearAtTheEnd.clear();
	rendererToClearAtTheEnd.shrink_to_fit();


	for(SDL_PixelFormat* fmt : pixelFormatToClearAtTheEnd)
	{
		if(fmt)
		{
			if(show_comments)
				std::printf("- pixel format, addr -> %p\n", fmt);

			SDL_FreeFormat(fmt);
		}		
	}
	pixelFormatToClearAtTheEnd.clear();
	pixelFormatToClearAtTheEnd.shrink_to_fit();
	
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


void lastError(std::string error)
{
	_lastError_ = error;
}


std::string str(const char* c_chaine)
{
	return std::string(c_chaine);
}


void swap(int& a, int& b)
{
	int tmp = a; 
	a = b;
	b = tmp; 
}


void swap(float& a, float& b)
{
	float tmp = a; 
	a = b;
	b = tmp; 
}


bool isInInterval_in_in(float min, float max, float val)
{
	/*on teste si : "val ∈ [min ; max]" est vrai*/

	if(min>max)
		return false;

	if(min <= val && val <= max)
		return true;
	return false;
}


bool isInInterval_in_out(float min, float max, float val)
{
	/*on teste si : "val ∈ [min ; max[" est vrai*/

	if(min>max)
		return false;

	if(min <= val && val < max)
		return true;
	return false;
}


bool isInInterval_out_in(float min, float max, float val)
{
	/*on teste si : "val ∈ ]min ; max]" est vrai*/

	if(min>max)
		return false;

	if(min < val && val <= max)
		return true;
	return false;
}


bool isInInterval_out_out(float min, float max, float val)
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
































//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//________________________FONCTIONS  DU MIXER (MUSIQUES)______________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________



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
























//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//____________________________________________________________________________________
//_________________________FONCTIONS  DES  PARTICULES_________________________________
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
			Particle p;
			
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
					Particle& current = box[i];
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
						Particle current = box[i];
							SDL_RenderDrawPoint(game->ren(), current.x, current.y);
							
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
				lastError(std::string("|| Warning || in 'sm::ParticleBox::draw()', trying to draw particles with a `ParticleBox' witch doesn't have an affected `sm::Game' object"));
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







} //fin namespace sm