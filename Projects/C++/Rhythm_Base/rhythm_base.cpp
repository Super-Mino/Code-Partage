#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <SDL3/SDL.h>
#include <SDL3_GSlib-Text.hpp>
#include <SFML/Audio.hpp>

#define WIN_W 880
#define WIN_H 600

//===================================================================================

typedef uint64_t Beat;

class Beats_Container //Une file de beat/impact/rythme, unité de temps : microseconde
{
	public:
		enum Status{NOTHING, AUTOPLAYING};
		
		Beats_Container() = default;
		~Beats_Container() = default;
		
		void setPartition(const std::vector<Beat>& beats); //Le système ici est statique, c-à-d que l'ensemble des beats d'un morceau sont rajoutés d'un coup et non en flux progressif.
		
		bool autoPlay(void (*actionCallback)(void)); //Jouer les beats de manière automatique.
		
		void update(double dt); //Arg 'dt' en seconde.

	protected:
		
		std::vector<uint64_t> m_beats; //Les beats, représentés par leur temps d'arrivée (relatif au au début du morceau), dans l'ordre croissant.
		int m_curr_beat_idx=0; //Tête de lecture des beats.
		
		Status m_status=NOTHING;		

		uint64_t m_timer=0; //Chronomètre, qui commence au début du morceau (à l'appel de autoPlay()).
		
		void (*m_actionCallback)(void)=nullptr; //Un callback (optionnel (mais pas vraiment, on y verrait rien sinon)) à exécuter à chaque beat.
		
};

void Beats_Container::setPartition(const std::vector<Beat>& beats)
{
	m_beats = beats;
	
	//Organisation des beats par ordre croissant des temps.
	std::sort(m_beats.begin(), m_beats.end(), [](const Beat& a, const Beat& b){
		return a < b;
	});
}


bool Beats_Container::autoPlay(void (*actionCallback)(void))
{
	if(m_status != NOTHING) 
		return false; //Pour ne garder qu'une opération à fois.

	m_actionCallback = actionCallback;
	m_timer = 0; //Timer en microseconde.
	m_curr_beat_idx=0;
	
	m_status = AUTOPLAYING;
	return true;
}


void Beats_Container::update(double dt) //Arg 'dt' en seconde.
{
	if(m_status==NOTHING) return;

	if(m_status==AUTOPLAYING)
	{
		if(m_curr_beat_idx >= m_beats.size()) //Si tous les beats sont parcourus.
		{
			m_status=NOTHING;
			return;	
		}

		//Sinon

		const Beat& curr_beat = m_beats.at(m_curr_beat_idx);
		
		//On regarde si son temps est arrivé (ou passé).
		if(curr_beat <= m_timer)
		{
			m_curr_beat_idx++;
			
			if(m_actionCallback) m_actionCallback();
		}		
	}
	
	
	uint64_t µs_dt = static_cast<uint64_t>(dt*1000000.0); //Conversion : seconde en microseconde.
	m_timer += µs_dt;

}


//===================================================================================


struct Beats_Recorder
{
	bool m_is_recording = false;
	uint64_t m_timer=0; //En microseconde.
	std::vector<Beat> m_last_record;	

	void addBeat() //Lance l'enregistrement au premier beat (qui commence donc au temps 0).
	{
		if(not m_is_recording)
		{
			m_last_record.clear();
			m_timer = 0;
			m_is_recording = true;
		}
	
		m_last_record.push_back(m_timer);
	}

	void updateRecorder(double dt) //Arg 'dt' en seconde.	
	{
		if(not m_is_recording) return;

		uint64_t µs_dt = static_cast<uint64_t>(dt*1000000.0); //Conversion : seconde en microseconde.
		m_timer += µs_dt;
	}


	void stopRecording() {m_is_recording = false;}

	std::vector<Beat> getLastRecord() const {return m_last_record;}

	
		

};


//===================================================================================


gs::Text_Line util_ln;
gs::Rendered_Str_Bank str_bank;
gs::Font font;

Beats_Container beats;
Beats_Recorder b_recorder;

bool show_record = false;
std::vector<Beat> record;

float show_rect_timer = 0.f;
sf::SoundBuffer beat_sound_buff;
sf::Sound beat_sound(beat_sound_buff);


void Init(SDL_Window* win, SDL_Renderer* ren);void InputGestion(bool& boolvar);void Update(float dt);
void Draw(SDL_Renderer* ren, float dt);
void beatCallback();


int main() 
{
	gs::Game game; 
	game.InitSDLEngine(WIN_W, WIN_H, "Rhythm Base");
	if(not game.isGameInit())
	{
		std::cout << "game.InitSDLEngine() failled\n";
		return 1;
	}

	SDL_Window* win=game.win();	SDL_Renderer* ren=game.ren();
	ThisGame = &game;

	Init(win, ren);

	const std::filesystem::path filename = "ok";
	sf::Music music;
    if (!music.openFromFile("C:/prog_utils/Sounds/Free Sound Effects - Nature Sounds (Royalty free Non Copyrighted Sound Effects).wav")) 
	{ std::cout<<"Failed to open file\n"; return 1; }
    music.play();
    //while (music.getStatus() == sf::SoundSource::Playing) { sf::sleep(sf::milliseconds(100)); }
    //return 0;
	
	bool go = true;
	while(go)
	{
		game.fpsProcessBegin();
		game.processDeltaTime();
		SDL_RenderClear(ren);
		
		InputGestion(go);
		
		Update(game.dt());
		Draw(ren, game.dt());
		
		SDL_RenderPresent(ren);
		game.fpsProcessEnd(120); 
	
		gs::lastErrorAutomaticLog();
	}

	printf("App ended\n");
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();  

	return 0; 
} 




//=========================================================================
//=========================================================================

void Init(SDL_Window* win, SDL_Renderer* ren)
{	
	SDL_SetRenderDrawColor(ren, 230u, 80u, 145u, 255u);
	SDL_SetRenderDrawColor(ren, 255u, 255u, 255u, 255u);
	SDL_SetRenderDrawColor(ren, 60u, 2u, 20u, 255u);

	//===============================================
	std::string font_path = "C:/Users/julus/Dev/C++/Main/Projects/Text_editor/ress/Font/consola.ttf";
	if(not util_ln.initAll(ren, font, str_bank, font_path.c_str(), 35, gs::u32RGBA(20,24,15,255)))
	{
		std::cout << "Font init failled : " <<font_path<<"\n";
	}
	else
	{
		util_ln.setText("Coucou alphabet Jklm 0123456789 _[{");
		util_ln.setPos({200,200});
	}
	
	beats.setPartition({0, 479869, 920342, 1382060, 1861254, 2314438, 2773456, 3204641,
			 		  3697028, 3909318, 4142078, 4167163, 4369626, 4387164, 4615174, 4888523, 5086702, 5299171, 
						5325322, 5552842, 5796950, 6025596, 6261125, 6511984, 6724465, 6931533, 7170208, 7382617, 
						7857286, 8301767, 8775369, 9221632, 9702608, 10180266, 10596785, 11056805, 11353412, 
						11609959, 11915095, 12247324, 12265335, 12539858, 12798005, 13161612, 13427194, 13445222, 
						13722342, 14609762, 14893214, 15239879, 15485412, 15789526, 16111171, 16378074, 16652351, 
						17015509, 17242114, 18156888, 18647369, 19095985, 19554949, 20029330, 20485712, 20961947, 
						21362879, 21841144, 22061082, 22259773, 22528216, 22723315, 22955452, 23198278, 23425593, 
						23653693, 23915816, 24142179, 24336292, 24613279, 25508476, 26430679, 26873657, 27344447, 
						27765887, 28250503, 28532126, 28850671, 29124359, 29442938, 29739055, 29988442, 30276594, 
	30590652, 30833240, 31450609, 31692429, 32121265, 32567340, 33512615, 33675060, 34160260, 34936953, });


	
	bool success = beat_sound_buff.loadFromFile("C:/Users/julus/Dev/C++/Hosanna/Sounds/Jumping_down.wav");
	

}


void InputGestion(bool& boolvar)
{
	static SDL_Event event;
	while(SDL_PollEvent(&event))	
	{
		if(event.type == SDL_EVENT_QUIT) {
        	boolvar=false;  /* end the program, reporting success to the OS. */
    	}
		else
		if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN and event.button.button == SDL_BUTTON_LEFT)
		{
			//b_recorder.addBeat();
			beats.autoPlay(beatCallback);
		}
		else
		if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN and event.button.button == SDL_BUTTON_RIGHT)
		{
			//b_recorder.stopRecording();
			//record = b_recorder.getLastRecord();
			//show_record = true;
		}
		
	}

}




void Update(float dt)
{
	b_recorder.updateRecorder(dt);
	beats.update(dt);

	if(0)
	if(show_record)
	{
		show_record = false;
		
		std::cout << "\n{";
		for(Beat b : record)
		{
			std::cout << b << ", ";
		}
		std::cout << "}\n";
	}
}



void Draw(SDL_Renderer* ren, float dt)
{
	if(show_rect_timer > 0.f)
	{
		gs::drawRect(ThisGame->ren(), {400.f, 400.f, 60.f, 60.f}, {200.f,200.f,200.f, 255.f}, true);
		show_rect_timer-= dt;
	}
}


void beatCallback()
{
	show_rect_timer = 0.02f; //sec.
	
	if(beat_sound.getStatus() == sf::SoundSource::Status::Playing) beat_sound.stop();

	beat_sound.play();
}


















