#ifndef _SDL2_GSlib_HPP_
#define _SDL2_GSlib_HPP_
//version modifiée le 14/07/23 (à moins d'oubli de modifier cette ligne). Première version de cette lib : 18/11/22
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_mixer.h>
	#include <SDL2/SDL_image.h> 
	#include <iostream>
	#include <cstdlib>
	#include <cstdint>
	#include <cstring>
	#include <vector>
	#include <cstdio>
	#include <random>
	#include <string>
	#include <ctime>
	#include <cmath>
	#include <new>


#define GSLIB_VERSION "2.3.0"
#define GSLIB_MAJOR 2
#define GSLIB_MINOR 3
#define GSLIB_PATCH 0
	
namespace sm /*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*/
{

/* Documentation basilé au bas de la page */

#define _pi_ 3.1415926

typedef SDL_Color Color;

enum Signe 	{NEGATIVE, POSITIVE}; 
enum Axis 	{X_AXIS, Y_AXIS};

struct Vec2i
{
	int x=0, y=0;
};

struct Vec2ui
{
	unsigned int x=0, y=0;
};

struct Vec2f
{
	float x=0.f, y=0.f;
};

struct Vec2d
{
	double x=0, y=0;
};

struct Vec2b
{
	bool x=false, y=false;
};


//___________________________________________________________________________________________________
//___________________________GESTIONNAIRE GENERAL DU JEU_____________________________________________
//---------------------------------------------------------------------------------------------------


class Game 
{
	public : 

		Game();
			//--------------------------------------------
		~Game();
			//____________________________________________
		/*0*/	
		void InitSDLEngine(int window_w = 800, int window_h = 480, const char* window_title = "MyGame", int windowpos_x = SDL_WINDOWPOS_CENTERED, int windowpos_y = SDL_WINDOWPOS_CENTERED, Uint32 win_flags=SDL_WINDOW_SHOWN, Uint32 ren_flags=SDL_RENDERER_ACCELERATED);
		
		/*1*/	
		bool InitByAssociateTo(SDL_Window *W, SDL_Renderer *newR );
			//____________________________________________


		/*2*/	
		SDL_Renderer* ren() const;
		
		/*3*/	
		SDL_Window* win() const; 
		
		/*4*/	
		int getWinW();
		
		/*5*/	
		int getWinH();


		int getRenW(); 


		int getRenH(); 
		
		/*6*/	
		bool mixActive() const;
		
		/*7*/	
		float dt() const;

		/*8*/
		void setWinSize(int w, int h);
		


		/*9*/	
		void changeRenderer(SDL_Renderer *newR, bool destroy = false);
		
		/*A*/	
		SDL_Texture* loadImg(const char *pth); 
		
		/*B*/	
		bool drawTile(SDL_Texture* tileset, int x, int y, int destB, int srcX, int srcY, int srcB, bool isRenderTheTarget = true);



		/*C*/	
		bool initMixer(int frequency = 96000, Uint16 format = MIX_DEFAULT_FORMAT, 
					   int channels =  MIX_DEFAULT_CHANNELS, int chunksize = 1024 );
		/*D*/	
		void closeMixer();
		
		/*E*/	
		void incrFree_channel(int val);
		
		/*F*/	
		void incrChannel_nb(int val);
		
		/*G*/	
		int getFree_channel();

		/*H*/	
		void delay(unsigned int &frameLimit, int fps = 60) const;
		
		/*I*/	
		void deltaTime();


		/*J*/	
		std::string lastError() const;
		
		/*K*/	
		void lastError(std::string error);


		void addToClearAtTheEnd(SDL_Texture* tex);


		void addToClearAtTheEnd(SDL_Renderer* renderer);


		void addToClearAtTheEnd(SDL_PixelFormat* fmt);


		Uint32 getWinID() const;


		bool isCloseWinEvent(SDL_Event& ev);


		void setWinIcon(const char* path);

		
		void setWinTitle(const char* new_title);
		

		void setAsMainGame();


		void setIsMainGame(const bool b);




		
	private : 
		SDL_Window *win_{nullptr};
		SDL_Renderer *ren_{nullptr};
		SDL_Surface *win_icon{nullptr};
		
		int Win_w;
		int Win_h;
		int m_ren_w=0;
		int m_ren_h=0;
		bool isInit{false};
		bool is_main_game{true};
		bool mixActive_;
		bool initMix_is_allowed;
		int free_channel_;
		int channels_nb_;
		int allocated_channels;
		
		float dt_, lastTime;
		
};




















//--------------------------------------------------------------------------------------------
//____________________________________________________________________________________________
//________________________UTILS_______________________________________________________________



SDL_Texture* loadImg(SDL_Renderer *renderer, const char* path);

SDL_Texture* loadImgAndDeleteAColor(SDL_Renderer *renderer, const char *p, Color C);

void logMessage(const char *msg, const char *_getError = nullptr, bool isFatalError = false);

bool drawTile(SDL_Renderer* ren, SDL_Texture* target, SDL_Texture* tileset, 
					int x, int y, int destB, int srcX, int srcY, int srcB, 
					bool takeInToAccountTheTarget = true);

void addToClearAtTheEnd(SDL_Texture* tex);

void addToClearAtTheEnd(SDL_Renderer* ren); 

void addToClearAtTheEnd(SDL_PixelFormat* fmt);

void freeGivenResources();

double rad(double degre);

double degree(double rad);

int roundToInt(double x);

double paw2(double x);

int paw2(int x);

std::string lastError();

void lastError(std::string error);

std::string str(const char* c_chaine);

void swap(int& a, int& b);

void swap(float& a, float& b);

bool isInInterval_in_in(float a, float b, float val);

bool isInInterval_in_out(float a, float b, float val);

bool isInInterval_out_in(float a, float b, float val);

bool isInInterval_out_out(float a, float b, float val);

double euclideanDist(double x1, double y1, double x2, double y2);

int euclideanDist(int x1, int y1, int x2, int y2);


// bool initImGui(SDL_Window* win, SDL_Renderer* ren, sm::Game* g);

// bool initImGui(SDL_Window* win=nullptr, SDL_Renderer* ren=nullptr, sm::Game* g=nullptr);

// void ImGuiNewFrame();

// void renderTheImGuiFrame();

// void quitImGui();














//--------------------------------------------------------------------------------------------
//____________________________________________________________________________________________
//______________________LA MUSIQUE____________________________________________________________

class Music
{
	public:
		Music();

		~Music();

		bool load(Game *g, const char *file);
		
		void play(int boucle = 1);
		
		void stop();
		
		void pause();
		
		void resume();
		
		void setVolume(int vol = 0);
		
		void incrVolume(int val);

	private:
		Mix_Chunk *music;
		int selfChannel;
		int volume;
};















//____________________________________________________________________________________________
//___________________RANDOM ABSTRACTION_______________________________________________________


class RandEngine
{
	public :
		RandEngine(float _a, float _b)
		: re(time(nullptr)), a(_a), b(_b)
		{	};

		~RandEngine(){}

		std::default_random_engine& getRe() {return re;}
		virtual float get()=0;

	protected : 
		float a=0;
		float b=1.f;
		std::default_random_engine re;
};




class NormalRandEngine : public RandEngine
{
	public : 

		NormalRandEngine(float _moy, float _ecart)
		: RandEngine(_moy, _ecart)
		{	
			std::normal_distribution<float>::param_type p{a, b};

			distrib.param(p);
		};

		~NormalRandEngine(){};

		float get() override {return distrib(re);} 

	private : 

		std::normal_distribution<float> distrib;
};




class UniformRandEngine : public RandEngine
{
	public : 
		UniformRandEngine(float _min, float _max)
		: RandEngine(_min, _max)
		{	
			std::uniform_real_distribution<float>::param_type p{a, b};

			distrib.param(p);
		};

		~UniformRandEngine(){};

		float get() override {return distrib(re);}

	private : 

		std::uniform_real_distribution<float> distrib;
};


















//____________________________________________________________________________________________
//___________________JEU DE PARTICULES________________________________________________________


struct Particle
{
	std::string type;
	short x, y;
	float vx, vy;
	float tmpvx, tmpvy;
	float gravity;
	float lifeTime;
	Particle * next;

};


class ParticleBox
{
	public : 
		ParticleBox() : re(time(nullptr)),   iks(300, 400), igrec(10, 80),
			 viks(-4, 4), vigrec(-4, 4), gravity(0.4, 0.6)
		{
			nb = 0; game = nullptr;

		};
		~ParticleBox() { clear(); std::cout<<"\n\tParticleBox destroyed succesfully"<<std::endl;};

		void init_game(Game *g, const char* path) 
		{game = g; tex = game->loadImg(path);}

		bool is_empty();
		void add_particle(std::string typ,
						  int xmin, int xmax, 
						  int ymin, int ymax, 
						  float vxmin, float vxmax,
						  float vymin, float vymax,
						  float gmin, float gmax,
						  float lifeT);
		void remove_back();
		void update();
		void draw();
		void clear();
		void print_nb(){std::cout<<"\n\tparticules : "<< nb<<std::endl;};

		SDL_Texture *tex;


	private :
		std::vector<Particle> box;
		std::default_random_engine re;
		int nb;
		Game *game;

		std::uniform_int_distribution<int>    iks, igrec;
		std::uniform_real_distribution<float> viks, vigrec,
												  gravity;

};





extern bool game_started;
extern bool game_won;
extern bool end_game;
extern bool show_comments;

};/*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*/

extern sm::Game *ThisGame;


#endif




 /* #0 - 	Initialise la SDL par défaut avec :
 			SDL_INIT_VIDEO - SDL_WINDOW_SHOWN - SDL_RENDERER_ACCELERATED
 			Aucun SDL_Window* ou SDL_Renderer* n'est requi.

 			- - - - -

 	#1 		S'initialise en s'associant avec un SDL_Window* et un SDL_Renderer* externes
 			à lui-même. Aucunes opérations ne sont effectuées dessus, c'est une 
 			simple association.

 			- - - - -

 	#2 		Renvoi le SDL_Renderer* auquel est associé.

 			- - - - -

 	#3 		Renvoi le SDL_Window* auquel est associé.			

			- - - - -

 	#4 		Renvoi la largeur de la fenêtre auquel est associé.			
			
			- - - - -

 	#5 		Renvoi la hauteur de la fenêtre auquel est associé.			
			
			- - - - -

 	#6 		Renvoi l'état d'activation du mixer.			
			
			- - - - -

 	#7 		Renvoi le delta time (temps écoulé entre les deux denières frames).			
			
			- - - - -

 	#8 		Permet d'attribuer une valeur aux dimensions de la fenêtre.
 			Cela ne fait rien d'autre, ne modifie pas les vraies dimensions de 
 			la fenêtre affichée.
 			Cela sert notamment	lorsque l'initialisation se fait par association,
 			afin de permettre à la variable `Game' d'avoir une trace des dimensions.	



 			suite de la doc plus tard	

*/

