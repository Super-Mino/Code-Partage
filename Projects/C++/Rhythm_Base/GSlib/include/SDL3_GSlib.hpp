#ifndef _SDL3_GSlib_HPP_
#define _SDL3_GSlib_HPP_
//27/08/24
//version modifiée le 30/11/25 (à moins d'oubli de modifier cette ligne ou autre). Première version de cette lib : 18/11/22
/*
Mises à jour:
22/07/23:	Ajout de 'Vec2si' et 'Vec2sui', update des 'drawTile()' partout.
03/08/23:	Ajout de 'col()',  'sign()'. 
06/08/23:	Ajout de 'strToupper()'.
17/08/23: 	Ajout de 'drawRect()', surchages de 'operator==' et de 'operator<<'.
09/05/24:	Ajout de 'Game::RenderPresent()' et 'Game::RenderClear()' .
22/01/25:	Ajout des 'decimalPart()'.
16/02/25:	-> 'perlinNoise2D()'.
19/10/25:   Passage à la SDL3.

*/


/* Documentation basilé au bas de la page (incomplète) */

	#include <SDL3/SDL.h>
	//#include <SDL3/SDL_mixer.h>
	#include <SDL3/SDL_image.h> 
	#include <unordered_map>
	#include <iostream>
	#include <cstdlib>
	#include <cstdint>
	#include <cstring>
	#include <vector>
	#include <cstdio>
	#include <random>
	#include <memory>
	#include <string>
	#include <cctype>
	#include <ctime>
	#include <cmath>
	#include <new>

	


#define GSLIB_VERSION "2.6.0"
#define GSLIB_MAJOR 2
#define GSLIB_MINOR 6
#define GSLIB_PATCH 0


	
namespace gs /*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*/
{




#define _pi_ 3.141592653589793238462643383279502884197169399375105820974944
#define _2pi_ 6.28318530718

typedef SDL_Color Color;

enum Signe 	{NEGATIVE=-1, POSITIVE=1}; 
enum Axis 	{X_AXIS, Y_AXIS};

#define INT_ERR_VAL -1



template<class T>
struct Vec2
{
	T x=static_cast<T>(0);
	T y=static_cast<T>(0);

	SDL_Point toSDL_Point() const;
	SDL_FPoint toSDL_FPoint() const;

	template<class U>
	Vec2<U> to() const;
	

	Vec2<T> operator+(const Vec2<T>& right) const;
	Vec2<T> operator-(const Vec2<T>& right) const;
	Vec2<T> operator*(const Vec2<T>& right) const;
	Vec2<T> operator/(const Vec2<T>& right) const;
	Vec2<T> operator%(const Vec2<T>& right) const;

	Vec2<T>& operator+=(const Vec2<T>& right);
	Vec2<T>& operator-=(const Vec2<T>& right);
	Vec2<T>& operator*=(const Vec2<T>& right);
	Vec2<T>& operator/=(const Vec2<T>& right);
	Vec2<T>& operator%=(const Vec2<T>& right);

	bool operator==(const Vec2<T>& right) const;
	bool operator!=(const Vec2<T>& right) const;

	Vec2<T> operator+(const T& val) const;
	Vec2<T> operator-(const T& val) const;
	Vec2<T> operator*(const T& val) const;
	Vec2<T> operator/(const T& val) const;
	Vec2<T> operator%(const T& val) const;

	Vec2<T>& operator+=(const T& val);
	Vec2<T>& operator-=(const T& val);
	Vec2<T>& operator*=(const T& val);
	Vec2<T>& operator/=(const T& val);
	Vec2<T>& operator%=(const T& val);

};




typedef Vec2<int> Vec2i;
typedef Vec2<short> Vec2si, Vec2s;
typedef Vec2<unsigned int> Vec2ui, Vec2u;
typedef Vec2<unsigned short> Vec2usi, Vec2us;

typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;

typedef Vec2<bool> Vec2b;
typedef Vec2<char> Vec2c;
typedef Vec2<unsigned char> Vec2uc;



struct Vec_Bool_Int 
{
	bool b_attr=false;
	int i_attr=0;
};


template<class T>
T dotProd(const Vec2<T>& vec1, const Vec2<T>& vec2);

template<class T>
T vecLen(const Vec2<T>& vec);

template<class T>
T vecAngleRad(const Vec2<T>& vec1, const Vec2<T>& vec2);


class Input
{
	public:

		Input() = default;
		~Input() = default;

		bool& operator[](std::string id);
		bool operator[](std::string id, bool new_state);

		std::unordered_map<std::string, bool>& getInputsMap() {return m_inputs;}


	private:
		std::unordered_map<std::string, bool> m_inputs;
};




class Mouse_Tracker
{
	public:
	
		Mouse_Tracker()=default;
		~Mouse_Tracker()=default;
		
		void processEvent(SDL_Event& ev);
		void update();
		
		bool isPreviouslyDown() const;
		bool isDown() const;
		Vec2f getPosWhenDown() const;
		Vec2f getPosWhenUp() const;
		Vec2f getPos() const;
		bool moved() const;
		bool last2DownClicksHaveSamePos() const;
		bool isMultiClick(char* out__clicks=nullptr) const;
		bool isDoubleClick() const;
		void incrMultiClickCount(char val);
		void setTargetBtn(bool is_left);
		bool targetBtnIsLeft() const;
		bool clickedDown() const;
		bool clickedUp() const;
		
		
	protected:
		
		bool       m_target_btn_is_left=true;
		gs::Vec2f  m_pos_when_down;
		bool       m_is_down=false;
		gs::Vec2f  m_pos_when_up;
		bool       m_previously_down=false;
		char       m_next_time_operation=0; //0: rien, 1: activer 'previously_down', 2: désactiver 'previously_down'.
		gs::Vec2f  m_pos; //la position courante
		bool       m_is_multi_click=false;
		char       m_clicks_count=0;
		gs::Vec2f  m_prev_pos; //Sert à savoir s'il y a eu mouvement depuis.
		gs::Vec2f  m_prev_down_pos; //Sert à savoir si la souris a clické à deux endroits différents sur les deux derniers clicks.
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
		void InitSDLEngine(int window_w = 800, int window_h = 480, const char* window_title = "MyGame", Uint32 win_flags=0);
		
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

		float getRenW_f(); 

		float getRenH_f(); 
		/*6*/	
		bool mixActive() const;
		/*7*/	
		double dt() const; //Nécessite 'Game::processDeltaTime()' dans la boucle principale.
		/*8*/
		void setWinSize(int w, int h);
		/*9*/	
		void changeRenderer(SDL_Renderer *newR, bool destroy = false);
		/*A*/	
		SDL_Texture* loadImg(const std::string& pth); 
		/*B*/	
		bool drawTile(SDL_Texture* tileset, int x, int y, int dest_w, int dest_h, int srcX, int srcY, int src_w, int src_h, bool isRenderTheTarget = true);
		/*C*/	
//bool initMixer(int frequency = 96000, Uint16 format = MIX_DEFAULT_FORMAT, 
			//		   int channels =  MIX_DEFAULT_CHANNELS, int chunksize = 1024 );
		/*D*/	
//void closeMixer();
		/*E*/	
//void incrFree_channel(int val);
		/*F*/	
//void incrChannel_nb(int val);
		/*G*/	
//int getFree_channel();
		/*H*/	
		void fpsProcessBegin();
		void fpsProcessEnd(Uint64 fps = 60);
		/*I*/	
		void processDeltaTime();
		/*J*/	
		std::string lastError() const;
		/*K*/	
		void lastError(std::string error);

		void addToClearAtTheEnd(SDL_Texture* tex);

		void addToClearAtTheEnd(SDL_Renderer* renderer);

		Uint32 getWinID() const;

		bool isCloseWinEvent(SDL_Event& ev);

		void setWinIcon(const std::string& path);
		
		void setWinTitle(const std::string& new_title);
		
		void setAsMainGame();

		void setIsMainGame(const bool b);

		void setUserData(void* data);

		void* getUserData();

		void renPresent(); 

		void renClear();

		SDL_Color setDrawColor(SDL_Color new_color);

		SDL_Color getDrawColor();

		int renX(float percent);

		int renY(float percent);

		bool isGameInit() const {return isInit;}



		
	protected : 
		SDL_Window *win_{nullptr};
		SDL_Renderer *ren_{nullptr};
		SDL_Surface *win_icon{nullptr};
		
		int Win_w;
		int Win_h;
		int m_ren_w=0;
		int m_ren_h=0;
		bool isInit{false};
		bool is_main_game{true};
		//bool mixActive_;
		//bool initMix_is_allowed;
		//int free_channel_;
		//int channels_nb_;
		//int allocated_channels;
		
		double dt_, lastTime;
		uint64_t m_frame_begin_ticks{0};
		uint64_t m_frame_end_ticks{0};
		bool m_fpsProcessBegin_is_ok=false;
		uint64_t m_last_time_ns=0;
		void* m_user_data=nullptr;
		
};




















//--------------------------------------------------------------------------------------------
//____________________________________________________________________________________________
//________________________UTILS_______________________________________________________________



SDL_Texture* loadImg(SDL_Renderer *renderer, const std::string& path);

SDL_Texture* loadImgAndDeleteAColor(SDL_Renderer *renderer, const char *p, Color C);

void logMessage(const char *msg, const char *_getError = nullptr, bool isFatalError = false);

bool drawTile(SDL_Renderer* ren, SDL_Texture* target, SDL_Texture* tileset, 
					int x, int y, int dest_w, int dest_h, int srcX, int srcY, int src_w, 
					int src_h, bool takeInToAccountTheTarget = true);

void addToClearAtTheEnd(SDL_Texture* tex);

void addToClearAtTheEnd(SDL_Renderer* ren); 

void freeGivenResources();

double rad(double degre);

double degree(double rad);

int roundToInt(double x);

double paw2(double x);

int paw2(int x);

std::string lastError();

void lastError(const std::string& str_info, const std::string& str_info_2="", const std::string& str_info_3="");

bool lastErrorAutomaticLog();

std::string str(const char* c_chaine);

bool isInInterval_in_in(double a, double b, double val);

bool isInInterval_in_out(double a, double b, double val);

bool isInInterval_out_in(double a, double b, double val);

bool isInInterval_out_out(double a, double b, double val);

double euclideanDist(double x1, double y1, double x2, double y2);
int euclideanDist(int x1, int y1, int x2, int y2);

double pointsDist(double x1, double y1, double x2, double y2);
int pointsDist(int x1, int y1, int x2, int y2);

double pointsDist(const Vec2d& pt1, const Vec2d& pt2);
int pointsDist(const Vec2i& pt1, const Vec2i& pt2);


SDL_FRect frect(SDL_Rect rect);

SDL_Rect rect(SDL_FRect rect);

SDL_Rect rect(int x, int y, int w, int h);

SDL_Color color(int r, int g, int b, int a=255); 

SDL_Color color(float r, float g, float b, float a=1.f);

SDL_Color color(uint32_t ucolor); //RGBA

SDL_Color color(SDL_FColor col);

SDL_FColor fcolor(SDL_Color col);

SDL_FRect frect(int x, int y, int w, int h);

//SDL_FColor fcolor(int r, int g, int b, int a=255); 

SDL_FColor fcolor(float r, float g, float b, float a=255.f);

SDL_FColor fcolor(uint32_t ucolor); //RGBA

uint32_t u32RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a=255);

uint32_t u32RGBA(SDL_Color col);

uint32_t u32ABGR(uint32_t rgba);

uint32_t u32RGBA(uint32_t abgr);

uint32_t u32RGBA(SDL_FColor col);

short sign(double val);

short sign(int val);

std::string strToupper(std::string str);

std::vector<std::string> strToupper(std::vector<std::string> str_vec);

bool drawRect(SDL_Renderer* ren, const SDL_FRect* rect, const SDL_FColor* col=nullptr, const bool& fill=false, SDL_BlendMode* b_mode=nullptr);
bool drawRect(SDL_Renderer* ren, const SDL_FRect& rect, const SDL_FColor* col=nullptr, const bool& fill=false, SDL_BlendMode* b_mode=nullptr);

bool drawRect(SDL_Renderer* ren, const SDL_FRect* rect, const SDL_FColor& col, const bool& fill=false, SDL_BlendMode* b_mode=nullptr);
bool drawRect(SDL_Renderer* ren, const SDL_FRect& rect, const SDL_FColor& col, const bool& fill=false, SDL_BlendMode* b_mode=nullptr);

bool drawLine(SDL_Renderer* ren, const Vec2f& pt1, const Vec2f& pt2, const SDL_FColor& col, SDL_BlendMode* b_mode=nullptr);
//bool drawLine(SDL_Renderer* ren, const Vec2i& pt1, const Vec2i& pt2, const SDL_FColor& col, SDL_BlendMode* b_mode=nullptr);

double dt();

SDL_Renderer* ren();

SDL_Window* win();

double getAngle(double origin_x, double origin_y, double point_x, double point_y, bool clockwise=true);
double getAngle(const Vec2d& origin, const Vec2d& point, bool clockwise=true);

Vec2d rotatePoint(double center_x, double center_y, double point_x, double point_y, double angle_rad, bool clockwise=true);

bool drawPoint(SDL_Renderer* ren, const int& x, const int& y, const SDL_Color* col=nullptr, SDL_BlendMode* b_mode=nullptr);
bool drawPoint(SDL_Renderer* ren, const int& x, const int& y, const SDL_Color& col, SDL_BlendMode* b_mode=nullptr);

bool drawPoint(SDL_Renderer* ren, const Vec2i& pt, const SDL_Color* col=nullptr, SDL_BlendMode* b_mode=nullptr);
bool drawPoint(SDL_Renderer* ren, const Vec2i& pt, const SDL_Color& col, SDL_BlendMode* b_mode=nullptr);


uint8_t alphaByPercentage(float percent);

Vec2d orthoVecOf(const Vec2d& vector_);
Vec2i orthoVecOf(const Vec2i& vector_);
Vec2i orthoVecOf_i(const Vec2d& vector_); //Avec conversion en int.
Vec2d orthoVecOf_d(const Vec2i& vector_); //Avec conversion en double.

Vec2d normalizedVecOf(const Vec2d& vector_);
Vec2d normalizedVecOf(const Vec2i& vector_);

bool lineAndCircleIntersect(const double& line_a, const double& line_b, const double& line_c, 
	const Vec2d& cir_center, const double& cir_radius, Vec2d* out__pt1=nullptr, Vec2d* out__pt2=nullptr);
bool lineAndCircleIntersect_i(const double& line_a, const double& line_b, const double& line_c, 
	const Vec2d& cir_center, const double& cir_radius, Vec2i* out__pt1=nullptr, Vec2i* out__pt2=nullptr);

Vec2d orthoProj(const Vec2d& point, const double& line_a, const double& line_b, const double& line_c);
Vec2i orthoProj_i(const Vec2i& point, const double& line_a, const double& line_b, const double& line_c);

bool isInField(const gs::Vec2d& field_origin, const double& fov, const double& dir_rad, const double& field_radius,
				const gs::Vec2d& circ_origin, const double& circ_radius, 
				gs::Vec2d* out__closest_pt=nullptr, double* out__origin_proximity=nullptr);

double decimalPart(double val);
float decimalPart(float val);

float perlinNoise2D(float x, float y, unsigned int seed=0);

std::vector<Vec2i> bresenham(int x0, int y0, int x1, int y1); //I.A - Algorithme de Bresenham pour tracer une ligne entre deux points.


unsigned int countCharsUTF8_ByCharHeader(const char& ch, bool& is_ok);
unsigned int countCharsUTF8_ByCharHeader(const char& ch);
unsigned int countCharsUTF8(const std::string& str, bool& is_ok);
unsigned int countCharsUTF8(const std::string& str);


bool isFlagPresentIn(int combined_flags, int specifiq_flag);

double changeRange(double val, double origin_min, double origin_max, double target_min, double target_max);



//--------------------------------------------------------------------------------------------
//____________________________________________________________________________________________
//______________________LA MUSIQUE____________________________________________________________

/*
class Music
{
	public:
		Music();

		~Music();

		bool load(Game *g, const char *file);
		
		void play(int boucle = 0);
		
		void stop();
		
		void pause();
		
		void resume();
		
		void setVolume(int vol = 0);
		
		void incrVolume(int val);

	protected:
		Mix_Chunk *music;
		int selfChannel;
		int volume;
};


*/












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

	protected : 

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

	protected : 

		std::uniform_real_distribution<float> distrib;
};


















//____________________________________________________________________________________________
//___________________JEU DE PARTICULES________________________________________________________

//old
struct pParticle
{
	std::string type;
	short x, y;
	float vx, vy;
	float tmpvx, tmpvy;
	float gravity;
	float lifeTime;
	pParticle * next;

};

//old
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


	protected :
		std::vector<pParticle> box;
		std::default_random_engine re;
		int nb;
		Game *game;

		std::uniform_int_distribution<int>    iks, igrec;
		std::uniform_real_distribution<float> viks, vigrec,
												  gravity;

};

















template<typename Distr_Type=std::normal_distribution<double>,
		typename Engine_Type=std::default_random_engine>
class Random  
{
	public :

		Random() { std::random_device rd; m_re.seed(rd()); };
		Random(uint32_t seed) { m_re.seed(seed); };
		Random(uint32_t seed, const Distr_Type::param_type& p) 
		{
			m_re.seed(seed); 
			m_distr.param(p);
		};

		virtual ~Random()=default;
		
		Distr_Type::result_type get() //Résultat avec les paramêtres données.
		{
			return m_distr(m_re);
		}


		//Ces deux méthodes sont particulièrement distinées pour les models de distributions normal et uniform.
		void initForFormattedGet(double mean_or_min, double stddev_or_max, const std::string& str_distr_type)
		{
			//Ces paramêtres donnés sont supposés être les mêmes que les vrais, afin d'avoir le comportement souhaité. 
			m_param_a = mean_or_min;
			m_param_b = stddev_or_max;
			m_distr_type = str_distr_type; //Soit "n" ou "normal", soit "u" ou "uniform".
		}

		Distr_Type::result_type get(double mean_or_min, double stddev_or_max) //Résultat formaté par les paramêtres reçus ici.
		{
			//Cette méthode permet de "générer avec des paramètres différents" sans avoir à avoir plusieurs instances de la classe, ou de changer les paramètres bruts de l'instance.

			if(m_distr_type==std::string("u") or m_distr_type==std::string("uniform"))
			{
				double& min = mean_or_min;
				double& max = stddev_or_max;

				double&& val = get(); //Prise de la valeur psuedo aléatoire avec les paramêtres déjà présents.
				double&& percent = (val-m_param_a) / (m_param_b-m_param_a); //Pourcentage de son éloignement au minimum en fonction du maximum.

				double formatted_val = min + (max-min)*percent; //Application avec les paramêtres ici demandés.
				return static_cast<Distr_Type::result_type>(formatted_val);
			}
			else
			if(m_distr_type==std::string("n") or m_distr_type==std::string("normal"))
			{ 
				double& mean = mean_or_min;
				double& stddev = stddev_or_max;

				double&& val = get(); //Prise de la valeur psuedo aléatoire avec les paramêtres déjà présents.
				double&& percent = (val-m_param_a) / (m_param_b); //Pourcentage de son éloignement au minimum en fonction du maximum.

				double formatted_val = mean + stddev*percent; //Application avec les paramêtres ici demandés.
				return static_cast<Distr_Type::result_type>(formatted_val);
			}
			else
				gs::lastError("||Warning|| in 'Random::get(double, double)', use 'Random::initForFormattedGet()' with right parameters and specify \"u\" or \"uniform\" _OR_ \"n\" or \"normal\". E.g : one_rand_engine.initForFormattedGet(one_rand_engine.getParam().mean(), one_rand_engine.getParam().stddev(), \"n\");.");

			return static_cast<Distr_Type::result_type>(-1);
		}


		void setParam(const Distr_Type::param_type& p)
		{
			m_distr.param(p);
		}


		Distr_Type::param_type getParam()
		{
			return m_distr.param();
		}



		void setSeed(uint32_t seed)
		{
			m_re.seed(seed);
		}

		
	protected :

		Distr_Type 		m_distr;
		Engine_Type 	m_re;

		//Ci dessous : prevu pour les usages des distributions normal et uniform.
		std::string 	m_distr_type="not_init";
		double 			m_param_a;
		double 			m_param_b;
		
};




enum Particle_T{RECT_PARTICLE, ELLIPSE_PARTICLE, CIRCLE_PARTICLE, TEXTURED_PARTICLE};
enum Rand_Distr_T{NORMAL_DISTR, UNIF_DISTR};
enum Particle_Coloring_T{RANDOM_CHOICE, TARGET_COLORS};
enum Particle_Attrib_Field_T{POS_X=0, POS_Y=1, SIZE_X=2, SIZE_Y=3, DIR=4, SPEED=5, TIME=6, COLOR=7, TEXTURE=7, SRC_RECT=7, ROT=7, ROT_SPEED=7};

template<typename User_Data_T=char>
struct Particle
{
	Particle_T type=ELLIPSE_PARTICLE;
	
	Vec2f pos={10.f,10.f};
	Vec2f size={8.f,8.f};
	float dir=0; 					//Direction : [0; 2pi].
	float speed=20; 				//Pixels par seconde.
	float left_time=10.f; 			//"Durée de vie" restante de la particule. 
	float begin_left_time=10.f; 	//"Durée de vie" de la particule.
	
	SDL_FColor color={0.f,200.f,201.f,255.f};
	User_Data_T udata;

	// void (*updater)(Particle& self, double dt)=nullptr;

	virtual void update(double dt);
	bool show(SDL_Renderer* ren, const bool& fill=true,  SDL_BlendMode* b_mode=nullptr);
	void setLeftTime(const float& t);

};



template<typename User_Data_T=char>
struct Textured_Particle : public Particle<User_Data_T>
{
	Particle_T type=TEXTURED_PARTICLE;

	SDL_Texture* texture=nullptr;
	SDL_FRect src_rect={0.f,0.f,10000.f,10000.f};
	float rotation=0; //Rotation de la texture [0, 2pi]. 
	float rotation_speed=0;

	User_Data_T udata;


	bool show(SDL_Renderer* ren);
	void update(double dt);
	void setLeftTime(const float& t);

};





struct Particle_Param
{
	struct Field {
		float a=0, b=0; //a = min ou mean; b = max ou stddev.
		Rand_Distr_T distr_type=NORMAL_DISTR;
	}
		x_pos={400, 10}, //Par exemple pour une distribution normale ça représente {mean, stddev}, et pour uniforme {min, max};	
		y_pos={250, 10},	
		x_size={7, 2},
		y_size={7, 2},
		dir={0.0, 2*_pi_},	
		speed={20, 8},	
		time={15, 5};

	void allTo(Rand_Distr_T type)
	{
		x_pos.distr_type = type;	
		y_pos.distr_type = type;	
		x_size.distr_type = type;
		y_size.distr_type = type;
		dir.distr_type = type;
		speed.distr_type = type;
		time.distr_type = type;
	}	
};

struct Particle_Color_Param
{
	struct Field {
		uint8_t a=0, b=0;
		Rand_Distr_T distr_type=UNIF_DISTR;
	}
		r_range={0, 255},
		g_range={0, 255},
		b_range={0, 255},
		a_range={255, 255};

	Particle_Coloring_T col_type=RANDOM_CHOICE;
	std::vector<SDL_Color> possible_colors;
	Rand_Distr_T possible_colors_distr_type;  

	void allTo(Rand_Distr_T type)
	{
		r_range.distr_type = type;	
		g_range.distr_type = type;	
		b_range.distr_type = type;
		a_range.distr_type = type;
	}

	void allTo(uint8_t a, uint8_t b)
	{
		r_range.a=a; r_range.b=b;
		g_range.a=a; g_range.b=b;
		b_range.a=a; b_range.b=b;
		a_range.a=a; a_range.b=b;
	}

};

template<typename P_User_Data_T=char>
void defaultParticleColorUpdater(const Particle<P_User_Data_T>& particle, SDL_Color& particle_color);


struct Particle_Texture_Param
{
	std::vector<SDL_Texture*> possible_textures;
	std::vector<SDL_Rect> possible_src_rects;

	Rand_Distr_T textures_distr_type=NORMAL_DISTR;
	Rand_Distr_T src_rects_distr_type=NORMAL_DISTR;

	struct Field {
		float a=0, b=0;
		Rand_Distr_T distr_type=NORMAL_DISTR;
	}
		rotation={0,2*_pi_},
		rotation_speed={0,0.12};
};


template<typename P_User_Data_T=char>
class Particle_Box
{
	public:
		virtual ~Particle_Box()=default;
		Particle_Box();

		void setSeed(Particle_Attrib_Field_T field, Rand_Distr_T distr_type, unsigned int seed); //Permet de déterminer les graines des différents générateurs de nombres pseudo-aléatoires. Par exemple pour le générateur uniforme de la taille en x, ou le générateur normal des couleurs, etc.
		unsigned int getSeed(Particle_Attrib_Field_T field, Rand_Distr_T distr_type) const;
		
		Particle_Param setParam(const Particle_Param& new_param);
		Particle_Param getParam() const;
		Particle_Param& getParam_ref();

		Particle_Color_Param setColorParam(const Particle_Color_Param& new_col_param);
		Particle_Color_Param getColorParam() const;
		Particle_Color_Param& getColorParam_ref();



		void addParticle(int count, Particle_Param* target_param=nullptr, Particle_T type=ELLIPSE_PARTICLE, const Particle_Color_Param* color_param=nullptr);
		void addParticle(const Particle<P_User_Data_T>& particle, int count=1);
		void addTexturedParticle(int count, Particle_Param* target_param, const Particle_Texture_Param& texture_param);
		void addTexturedParticle(const Textured_Particle<P_User_Data_T>& particle, int count=1);

		void update(double dt);
		void show(SDL_Renderer* ren);

		void setParticleColorUpdater(void (*particleColorUpdater)(const Particle<P_User_Data_T>& particle, SDL_Color& particle_color));
		void setParticleBehaviorUpdater(void (*particleBehaviorUpdater)(Particle<P_User_Data_T>& particle, double dt));
		void setTParticleBehaviorUpdater(void (*tParticleBehaviorUpdater)(Textured_Particle<P_User_Data_T>& particle, double dt));

		uint32_t getCount() const; //La totale. 
		uint32_t getCount(Particle_T type) const; //Au choix.

	protected:

		void adjustParam(const Particle_Param& param);
		float getRandVal(int field_index, const Particle_Param& param);
		SDL_Color getColor(const Particle_Color_Param& cp);
		SDL_Texture* getTexture(const Particle_Texture_Param& tp);
		SDL_Rect getSrcRect(const Particle_Texture_Param& tp);
		Vec2f getRotAndRotSpeed(const Particle_Texture_Param& tp);


		std::vector<Random<std::normal_distribution<float>>> 		m_norm_distrs;
		std::vector<Random<std::uniform_real_distribution<float>>> 	m_unif_distrs;
	
		std::vector<Particle<P_User_Data_T>> 						m_particles;
		std::vector<Textured_Particle<P_User_Data_T>>  				m_t_particles;
		Particle_Param 												m_param;
		Particle_Color_Param 										m_col_param;

		std::vector<gs::Vec2ui> 									m_seeds; //L'odre (arbitraire) est : 'm_seeds[i].x' = graine du générateur 'normal', 'm_seeds[i].y' = graine du générateur 'uniform'.

		void (*m_particleColorUpdater)(const Particle<P_User_Data_T>& particle, SDL_Color& particle_color)=nullptr;
		void (*m_particleBehaviorUpdater)(Particle<P_User_Data_T>& particle, double dt)=nullptr;
		void (*m_t_particleBehaviorUpdater)(Textured_Particle<P_User_Data_T>& particle, double dt)=nullptr;


};











extern bool game_started;
extern bool game_won;
extern bool end_game;
extern bool show_comments;

};/*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*/

extern gs::Game *ThisGame;







std::ostream& operator<<(std::ostream& os, SDL_Rect& rect);
std::ostream& operator<<(std::ostream& os, SDL_FRect& rect);
std::ostream& operator<<(std::ostream& os, SDL_Point& point);
std::ostream& operator<<(std::ostream& os, SDL_FPoint& point);
std::ostream& operator<<(std::ostream& os, SDL_Color& col);

bool operator==(const SDL_Rect& rect1, const SDL_Rect& rect2);
bool operator==(const SDL_FRect& rect1, const SDL_FRect& rect2);
bool operator==(const SDL_Point& pt1, const SDL_Point& pt2);
bool operator==(const SDL_FPoint& pt1, const SDL_FPoint& pt2);
bool operator==(const SDL_Color& col1, const SDL_Color& col2);



#include "SDL3_GSlib-template.hpp"


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



 			Suite de la doc plus tard peut-être	(ou jamais).

*/

