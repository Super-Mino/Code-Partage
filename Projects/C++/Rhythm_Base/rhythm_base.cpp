#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <fstream>
#include <SDL3/SDL.h>
#include <SDL3_GSlib-Text.hpp>
#include <SFML/Audio.hpp>

#define WIN_W 880
#define WIN_H 600

//===================================================================================

enum Beat_Type{CONSTRUCTION, EXPECTED_RESPONSE};
enum Response_Type{CLICK_DOWN, CLICK_UP};

const int BEAT_STRUCT_Version = 1; //Pour la gestion de la compatibilité dans les sauvegardes.
struct Beat
{
	//version 1
	uint64_t time = 0;
	Beat_Type type = CONSTRUCTION;
	Response_Type resp_type = CLICK_DOWN;
};

struct Beats_Editor 
{
	//Aligne les bâtements sur les beats du métronome (ou de leurs quotients si 'metronome_beat_div > 1'), et retire les doublons.
	static std::vector<Beat> align(std::vector<Beat> beats, double bpm, uint32_t metronome_beat_div = 1) 
	{
		//Toutefois, il est supposé à l'entrée de cette méthode que les beats sont déjà classés par ordre croissant des temps.
		try{

		if(metronome_beat_div == 0) metronome_beat_div = 1;
		
		double one__metro_beat_dur = 60000000.0 / bpm; //L'écart de temps, en µs, entre deux bâtements de métronome.		
		uint64_t unit_duration = one__metro_beat_dur / static_cast<double>(metronome_beat_div);	
		
		//Alignement.
		for(Beat& b : beats)
		{
			//On détermine l'alignement le plus proche entre celui avant et celui après lui.
			
			uint64_t min_time = (b.time/unit_duration)*unit_duration;
			uint64_t remain = b.time % unit_duration;
			
			if(remain < unit_duration/2u)
				b.time = min_time;
			else
				b.time = min_time+unit_duration;
		}


		//Suppression des doublons.
		uint64_t prev_beat_time;
		bool just_started = true; 
		for(int i=static_cast<int>(beats.size())-1; i>=0; --i)
		{
			Beat& b = beats.at(i);

			if(just_started) //Pour avoir au moins un premier beat de comparaison.
			{
				prev_beat_time = b.time;
				just_started = false;
				continue;
			}
			
			if(b.time == prev_beat_time)
			{
				beats.erase(beats.begin()+i+1); //Retrait de la précédente.
			}

			prev_beat_time = b.time;
		}
		
		}
		catch(std::exception& excep){std::cout << "Excep Beats_Editor::align() : " << excep.what()<< "\n";}
		
		return beats;
	}	

};


//===================================================================================


struct Beats_Viewer
{
	uint64_t m_timer=0; // µs
	std::vector<Beat> m_beats;
	uint32_t m_curr_beat_idx=0; //L'indice du prochain bâtement à lire.
	bool m_is_playing = false;
	double one_pix_duration=20000.0/2.0; //C'est la graduation temporelle : l'écart d'un pixel à l'écran représente une durée de 'one_pix_duration' µs. Pour une valeur de 20000µs, 1s s'étand sur 50 pixels. 

	SDL_FRect m_area = {20.f, 200.f, 750.f, 150.f};
	SDL_FColor m_bg_col = {60.f,70.f,80.f,255.f};
	SDL_FColor m_beat_col = {200.f,200.f,200.f,255.f};
	SDL_FColor m_head_col = {0.f,200.f,0.f,255.f}; //Couleur de la tête de lecture.
	SDL_FColor m_grad_col = {0.f,0.f,0.f,255.f}; //Couleur des graduations des bmp.

	uint32_t m_min_visible_beat_idx=0; //Le beat par le quelle commencer, celui le plus à gauche (= ceux avant lui sont hors cadre). Ca permet de réduire le nombre de traitement. 
	double m_bpm = 120;

	void setPartition(const std::vector<Beat>& beats)
	{
		m_beats = beats;
		m_min_visible_beat_idx = 0;
		m_curr_beat_idx = 0;
	}

	bool play(uint32_t bpm)
	{
		if(m_is_playing) return false;
		
		m_is_playing = true;
		m_timer = 0;
		m_min_visible_beat_idx = 0;
		m_curr_beat_idx = 0;
		m_bpm = bpm;

		return true;
	}

	bool isPlaying() const {return m_is_playing;}

	void stop() {m_is_playing = false;}

	void updateAndShow(double dt, SDL_Renderer* ren)
	{
		gs::drawRect(ren, m_area, m_bg_col, true);
		
		gs::Vec2f head_top = {m_area.x+m_area.w/2.f, m_area.y};
		gs::Vec2f head_bottom = {m_area.x+m_area.w/2.f, m_area.y+m_area.h};
		gs::drawLine(ren, head_top, head_bottom, m_head_col);
		

		//Dessin des graduations.

		double one__bpm_beat_dur = 60000000.0 / m_bpm; //L'écart de temps, en µs, entre deux bâtements de métronome.		
		

		gs::drawLine(ren, {m_area.x, m_area.y+m_area.h*0.85f}, {m_area.x+m_area.w-1, m_area.y+m_area.h*0.85f}, m_grad_col);
		gs::drawLine(ren, {m_area.x, m_area.y+m_area.h*0.85f+1}, {m_area.x+m_area.w-1, m_area.y+m_area.h*0.85f+1}, m_grad_col);
		
		double timer_d = static_cast<double>(m_timer);
		double next_bpm_beat_time_r = -fmod(timer_d, one__bpm_beat_dur); //Pour les graduations "côté droit".
		double next_bpm_beat_time_l = next_bpm_beat_time_r; //Pour les graduations "côté gauche".

		int fisrt_drawn_grad_num = static_cast<int>(timer_d/ one__bpm_beat_dur); //Le numéro/rang de la graduation (relatif au début des temps). (Note: deux graduations ont un écrat de 1 (en beat de métronome)).


		//Les graduations du côté droit (côté non encore lu).
		for(int i=0; i<100; ++i) //Limite arbitraire du nombre max de double graduation.
		{
			//Déduction de la position de la graduation (relative au zéro) sur l'axe du temps (affiché à l'écran).
			float pos_in_time_axis_r = next_bpm_beat_time_r/one_pix_duration +head_top.x;
			float pos_in_time_axis_l = next_bpm_beat_time_l/one_pix_duration +head_top.x;
			
			float min_perc = 0.76f, max_perc = 0.9f;
			int curr_grad_num_r = fisrt_drawn_grad_num+i;
			int curr_grad_num_l = fisrt_drawn_grad_num-i;


			//"Côté droit"			

			if(pos_in_time_axis_r >= m_area.x+m_area.w) //Teste de la visibilité côté droit.
				break;

			//Posons "une période" comme étant la durée de 4 bâtements de métronome.
			if(curr_grad_num_r %4 == 0) //Ceci permet de mettre en relief les beats des débuts de périodes. 
			{
				min_perc = 0.68f; max_perc = 0.95f;
			}
			
			if(pos_in_time_axis_r >= m_area.x) //Teste de la visibilité côté gauche.
			{
				gs::Vec2f grad_top = {pos_in_time_axis_r, m_area.y+m_area.h*min_perc};
				gs::Vec2f grad_bottom = {pos_in_time_axis_r, m_area.y+m_area.h*max_perc};
				gs::drawLine(ren, grad_top, grad_bottom, m_grad_col);
				grad_top.x+=1;grad_bottom.x+=1; //Pour l'épaisseur du trait.
				gs::drawLine(ren, grad_top, grad_bottom, m_grad_col);
			}

			//Oui pour les graduations du "côté droit" on vérifie les limites et à droite et à gauche, car une gradutation de "droite" (une seule à la fois) peut se retouver du "côté gauche" de la tête de lecture. 


			//__________________________________________________________________________________//

			//"Côté gauche"
			

			min_perc = 0.76f; max_perc = 0.9f;
			if(curr_grad_num_l %4 == 0) //Ceci permet de mettre en relief les beats des débuts de périodes. 
			{
				min_perc = 0.68f; max_perc = 0.95f;
			}

			if(pos_in_time_axis_l >= m_area.x) //Teste de la visibilité côté gauche.
			{
				gs::Vec2f grad_top = {pos_in_time_axis_l, m_area.y+m_area.h*min_perc};
				gs::Vec2f grad_bottom = {pos_in_time_axis_l, m_area.y+m_area.h*max_perc};
				gs::drawLine(ren, grad_top, grad_bottom, m_grad_col);
				grad_top.x+=1;grad_bottom.x+=1; //Pour l'épaisseur du trait.
				gs::drawLine(ren, grad_top, grad_bottom, m_grad_col);
			}
			
			next_bpm_beat_time_r += one__bpm_beat_dur;
			next_bpm_beat_time_l -= one__bpm_beat_dur;
		}


		//Dessin des beats de la partition.	
		for(uint32_t i=m_min_visible_beat_idx; i<m_beats.size(); ++i)
		{
			const Beat& b = m_beats.at(i);

			double remaining_time = static_cast<double>(b.time) - static_cast<double>(m_timer); //En µs, le temps restant avant que le beat "résonne"/"sonne".
			
			//On en déduit alors la position (relative au zéro) sur l'axe du temps (affiché à l'écran).
			float pos_in_time_axis = remaining_time / one_pix_duration;
			pos_in_time_axis += head_top.x; //Déplacement vers l'origine de l'axe affiché.

			//On teste la visiblité
			if(pos_in_time_axis < m_area.x) //Côté gauche
			{
				m_min_visible_beat_idx++;
				continue;
			}
			else if(pos_in_time_axis >= m_area.x+m_area.w) //Côté droit
				break;

			//On met à jour la tête de lecture.
			if(i == m_curr_beat_idx and remaining_time < 0)
				m_curr_beat_idx++;
				

			gs::Vec2f b_top = {pos_in_time_axis, m_area.y+m_area.h*0.2f};
			gs::Vec2f b_bottom = {pos_in_time_axis, m_area.y+m_area.h*0.6f};
			
			if(i == m_curr_beat_idx)
				gs::drawLine(ren, b_top, b_bottom, {200.f,2.f,2.f,255.f});
			else
				gs::drawLine(ren, b_top, b_bottom, m_beat_col);

			//Si tout a été lu, on arrête la lecture quelque temps après.
			if(m_curr_beat_idx >= m_beats.size())
			{
				if(m_beats.size() > 0)
				{
					const Beat& last_b = m_beats.back();
					double l_b_time = last_b.time;
					double t = m_timer;
					double past = abs(l_b_time - t);
					if(past > 1000000.0) //1s
						m_is_playing = false;
				}
				else
					m_is_playing = false;
			}
			


		}

		if(not m_is_playing) return;

		uint64_t µs_dt = static_cast<uint64_t>(dt*1000000.0); //Conversion : seconde en microseconde.
		m_timer += µs_dt;
	}
};


//===================================================================================


class Beats_Container //Une file de beat/impact/rythme, unité de temps : microseconde
{
	public:
		enum Status{NOTHING, AUTOPLAYING};
		
		Beats_Container() = default;
		~Beats_Container() = default;

		bool setPartition(const std::string& file_path);
		
		void setPartition(const std::vector<Beat>& beats); //Le système ici est statique, c-à-d que l'ensemble des beats d'un morceau sont rajoutés d'un coup et non en flux progressif.
		std::vector<Beat> getPartition() const;

		bool autoPlay(void (*actionCallback)(void)); //Jouer les beats de manière automatique.
		
		void update(double dt); //Arg 'dt' en seconde.

		bool setMusic(const std::string& file_path);

	protected:
		
		std::vector<Beat> m_beats; //Les beats, représentés par leur temps d'arrivée (relatif au au début du morceau), dans l'ordre croissant.
		int m_curr_beat_idx=0; //Tête de lecture des beats.
		
		Status m_status=NOTHING;		

		uint64_t m_timer=0; //Chronomètre, qui commence au début du morceau (à l'appel de autoPlay()).
		
		void (*m_actionCallback)(void)=nullptr; //Un callback (optionnel (mais pas vraiment, on y verrait rien sinon)) à exécuter à chaque beat.
		
		sf::Music m_music;
		bool m_music_is_ready = false;	
};


bool Beats_Container::setPartition(const std::string& file_path)
{
	std::ifstream file(file_path, std::ios::in);

	if(not file) return false;
	
	try{

	m_beats.clear();
	
	int version = -1;
	file >> version;

	while(not file.eof())
	{
		Beat b;		

		if(version >= 1)
		{
			int type=0, resp_type=0;
			file >> b.time;
			file >> type; b.type = static_cast<Beat_Type>(type);
			file >> resp_type; b.resp_type = static_cast<Response_Type>(resp_type);
		}

		if(not file.good())
			return false;


		m_beats.push_back(b);		
	}
	
	}catch(std::exception& excep){ std::cout << "Excep in 'Beats_Container::setPartition()' : " << excep.what() << '\n'; return false;}	
	

	setPartition(m_beats); //Par sécurité pour l'ordre croissant des temps.
	
	return true;
}

void Beats_Container::setPartition(const std::vector<Beat>& beats)
{
	m_beats = beats;
	
	//Organisation des beats par ordre croissant des temps.
	std::sort(m_beats.begin(), m_beats.end(), [](const Beat& a, const Beat& b){
		return a.time < b.time;
	});
}

std::vector<Beat> Beats_Container::getPartition() const {return m_beats;}


bool Beats_Container::autoPlay(void (*actionCallback)(void))
{
	if(m_status != NOTHING) 
		return false; //Pour ne garder qu'une opération à fois.

	m_actionCallback = actionCallback;
	m_timer = 0; //Timer en microseconde.
	m_curr_beat_idx=0;

	if(m_music_is_ready)
	{
		m_music.stop();
		m_music.play();
	}
	
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

			if(m_music_is_ready)
				m_music.stop();

			return;	
		}

		//Sinon

		const Beat& curr_beat = m_beats.at(m_curr_beat_idx);
		
		//On regarde si son temps est arrivé (ou passé).
		if(curr_beat.time <= m_timer)
		{
			m_curr_beat_idx++;
			
			if(m_actionCallback) m_actionCallback();
		}		
	}
	else
		if(m_music_is_ready)
			m_music.stop();
	
	
	uint64_t µs_dt = static_cast<uint64_t>(dt*1000000.0); //Conversion : seconde en microseconde.
	m_timer += µs_dt;

}


bool Beats_Container::setMusic(const std::string& file_path)
{
	if(not m_music.openFromFile(file_path))
		return false;

	//m_music.setVolume(50.f);

	m_music_is_ready = true;
	return true;
}

//===================================================================================


struct Beats_Recorder
{
	bool m_is_recording = false;
	uint64_t m_timer=0; //En microseconde.
	std::vector<Beat> m_last_record;
	sf::Music m_music;
	bool m_music_is_ready = false;

	bool setMusic(const std::string& file_path)
	{
		if(not m_music.openFromFile(file_path))
			return false;

		m_music_is_ready = true;
		return true;
	}

	void addBeat(bool record_first_beat, Beat b_model) //Lance l'enregistrement au premier beat (qui commence donc au temps 0).
	{
		if(not m_is_recording)
		{
			m_last_record.clear();
			m_timer = 0;
			m_is_recording = true;
			
			if(m_music_is_ready)
			{
				m_music.stop();
				m_music.play();
			}
			

			if(not record_first_beat)
				return;
		}
		
		b_model.time = m_timer;
		m_last_record.push_back(b_model);
	}

	void updateRecorder(double dt) //Arg 'dt' en seconde.	
	{
		if(not m_is_recording) return;
		uint64_t µs_dt = static_cast<uint64_t>(dt*1000000.0); //Conversion : seconde en microseconde.
		m_timer += µs_dt;
	}


	void stopRecording() 
	{
		m_is_recording = false;
		
		if(m_music_is_ready)
			m_music.stop();
	}

	std::vector<Beat> getLastRecord() const {return m_last_record;}

	bool isRecording() const {return m_is_recording;}	


	bool saveLastRecord(const std::string& file_path) const
	{
		std::ofstream file(file_path, std::ios::out);
		
		if(not file) return false;

		//Organisation du fichier : 
		//Note: mode == fichier texte

		// En premier : version
		file << BEAT_STRUCT_Version << "\n";
		
		//S'en suit les différentes attributs d'un beat, dans l'ordre qui suit, pour tous les beats.
		for(const Beat& b : m_last_record)
		{
			file << b.time << ' ';
			file << static_cast<int>(b.type) << ' ';
			file << static_cast<int>(b.resp_type) << ' ';
			file << '\n';
		}
		
		return true;
	}
};


//===================================================================================


gs::Text_Line util_ln;
gs::Rendered_Str_Bank str_bank;
gs::Font font;

gs::Mouse_Tracker mouse_left;
gs::Mouse_Tracker mouse_right;
gs::Input input;

Beats_Container beats;
Beats_Recorder b_recorder;
Beats_Viewer b_viewer;


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

	
	//sf::Music music;
    //if (!music.openFromFile("C:/prog_utils/Sounds/Free Sound Effects - Nature Sounds (Royalty free Non Copyrighted Sound Effects).wav")) 
	//{ std::cout<<"Failed to open file\n"; return 1; }
    //music.play();
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

	mouse_left.setTargetBtn(true);
	mouse_right.setTargetBtn(false);

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
	
	//beats.setPartition(Beats_Editor::align({5027981, 8360996, 11668884, 14977630, 15825361, 16650542, 18295949, 19133109, 19964559, 21622652, 22476956, 23277727, 24937465, 25741417, 26280706, 26445256, 26614296, 27406620, 28238103, 29853669, 30670077, 31086239, 31565407, 32826357, 33230381, 34020329, 34860024, 35261139, 35672182, 36516154, 37329010, 38158067, 39018983, 39804075, 40652850, 41473028, 43918604, 44338749, 44773866, 46443529, 47223311, 47630980, 48119483, 49764144, 50579316, 50973398, 51408067, 54715042, 56376592, 57178966, 57598981, 58035610, 58852825, 59522068, 59672500, 60500721, 61352801, 62196619, 63408981, 63829070, 64211250, 64648390, 65874274, 66314364, 67111410, 67331982, 67501544, 67777178, 67935687, 68783915, 69610262, 70426651, 70722165, 70858172, 71265786, 72112072, 72916648, 73710925, 73989472, 74123958, 74569064, 75416146, 76230826, 76777415, 76973181, 77335276, 77459839, 77869017, 78712164, 79533925, 79931546, 80363556, 80777149, 81203808, }
											//, 145, 1));
	

	beats.setMusic("Ress/Audio/free/Rhythm_base_audio_test_1_145bpm.mp3");
	beats.setPartition("Beats_rhythm_test1_145bpm.txt");
	beats.setPartition(Beats_Editor::align(beats.getPartition(), 145, 1));
	b_viewer.setPartition(beats.getPartition());

	
	b_recorder.setMusic("Ress/Audio/free/Rhythm_base_audio_test_1_145bpm.mp3");
	
	bool success = beat_sound_buff.loadFromFile("C:/Users/julus/Dev/C++/Hosanna/Sounds/Jumping_down.wav");

}


void InputGestion(bool& boolvar)
{
	static SDL_Event event;
	while(SDL_PollEvent(&event))	
	{
		mouse_left.processEvent(event);
		mouse_right.processEvent(event);

		if(event.type == SDL_EVENT_QUIT) {
        	boolvar=false;  /* end the program, reporting success to the OS. */
    	}
		else
		if(event.type == SDL_EVENT_KEY_DOWN)
		{
			switch(event.key.key)
			{
			case SDLK_SPACE: input["space"] = true; break;
			case SDLK_S: input["s"] = true; break;
			default: break;
			}
				
		}
		else
		if(event.type == SDL_EVENT_KEY_UP)
		{
			switch(event.key.key)
			{
			case SDLK_SPACE: input["space"] = false; break;
			case SDLK_S: input["s"] = false; break;
			default: break;
			}
		}
		if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN and event.button.button == SDL_BUTTON_LEFT)
		{}
		else
		if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN and event.button.button == SDL_BUTTON_RIGHT)
		{}

		
	}

}




void Update(float dt)
{
	mouse_left.update();
	mouse_right.update();

	if(mouse_left.clickedDown())
	{
		beats.autoPlay(beatCallback);
		b_viewer.play(145);
	}
	
	
	beats.update(dt);
	

	if(0) //Côté enregistrement
	{
		if(input["space"])
		{
			b_recorder.stopRecording();
			b_recorder.saveLastRecord("Beats_rhythm_test1_145bpm.txt");

			b_viewer.stop();
			input["space"] = false;
		}
		
		if(mouse_left.clickedDown())
		{
			if(not b_viewer.isPlaying())
				b_viewer.play(145);

			Beat b; b.type = CONSTRUCTION;
			b_recorder.addBeat(false, b);
			
			b_viewer.setPartition(b_recorder.getLastRecord());
		}

		if(mouse_right.clickedDown())
		{
			if(not b_viewer.isPlaying())
				b_viewer.play(145);

			Beat b; 
			b.type = EXPECTED_RESPONSE;
			b.resp_type = CLICK_DOWN;
			b_recorder.addBeat(false, b);

			b_viewer.setPartition(b_recorder.getLastRecord());
		}
		
		
		b_recorder.updateRecorder(dt);


		//record = b_recorder.getLastRecord();
		//show_record = true;
		
		if(show_record)
		{
			show_record = false;
			
			std::cout << "\n{";
			for(Beat b : record)
			{
				std::cout << b.time << ", ";
			}
			std::cout << "}\n";
		}
	}
}



void Draw(SDL_Renderer* ren, float dt)
{
	if(show_rect_timer > 0.f)
	{
		gs::drawRect(ThisGame->ren(), {400.f, 400.f, 60.f, 60.f}, {200.f,200.f,200.f, 255.f}, true);
		show_rect_timer-= dt;
	}

	b_viewer.updateAndShow(dt, ren);
}


void beatCallback()
{
	show_rect_timer = 0.02f; //sec.
	
	if(beat_sound.getStatus() == sf::SoundSource::Status::Playing) beat_sound.stop();

	beat_sound.play();
}


















