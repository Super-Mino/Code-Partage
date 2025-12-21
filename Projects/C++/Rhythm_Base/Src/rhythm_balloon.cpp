#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <fstream>
#include <SDL3/SDL.h>
#include <SDL3_GSlib-Text.hpp>
#include <SDL3_GSlib-Geo.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include "anim.h"

#define WIN_W 880
#define WIN_H 600



//===================================================================================

struct Text_Looking_Params
{
	int style = TTF_STYLE_NORMAL;
	int ptsize = 25;
	SDL_Color fg_col={255,180,0,255};
	SDL_Color bg_col={0,0,0,0};
};


struct Game_Text
{
	std::unordered_map<std::string, std::string> txt{
		{"#too_early","Trop tôt"},
		{"#good", "Bien"},
		{"#nice", "Excellent !"},
		{"#too_late", "Trop tard"},
		{"#missed", "Manqué"}
	};

	std::unordered_map<std::string, std::string> look{
		{"#too_early","#look_0"},
		{"#good", "#look_1"},
		{"#nice", "#look_2"},
		{"#too_late", "#look_0"},
		{"#missed", "#look_0"}
	};

	std::unordered_map<std::string, Text_Looking_Params> looks_bank{
		{"#look_0", Text_Looking_Params(TTF_STYLE_NORMAL, 40, {255,50,0,255}, {0,0,0,0})},
		{"#look_1", Text_Looking_Params(TTF_STYLE_NORMAL, 50, {80,255,80,255}, {0,0,0,0})},
		{"#look_2", Text_Looking_Params(TTF_STYLE_NORMAL, 70, {255,190,0,255}, {0,0,0,0})}
	};
};



class Game_Text_Textures
{
	public:	

		Game_Text_Textures() = default;
		~Game_Text_Textures() {
			for(auto  [text, texture] : m_textures)
			{
				if(texture)
					SDL_DestroyTexture(texture);
			}
		}


		void loadTexts(gs::Font& font, const Game_Text& texts)
		{
			if(not font.isInit())
			{
				std::cout << "!! in 'Game_Text_Textures::loadTexts()', given font is not init. \n";
				return;
			}

			const int prev_font_style = font.getFontStyle(); //On les récupère pour pouvoir restaurer le font à l'état reçu. 
			const int prev_font_ptsize = font.getPtsize();

			for(const auto&  [ID, text] : texts.txt)
			{
				if(m_textures.contains(ID)) //Je considère ici qu'il n'y a pas lieu d'avoir des changements de texture pour un identifant dont la texture est déjà chargée. 
					continue; 


				//======================= Paramétrage de l'apparence du texte. ==================================================
				
				SDL_Texture* texture = nullptr;
				int style = TTF_STYLE_NORMAL; //Les paramètres d'apparence par défaut, pour le texte.
				int ptsize = 25;
				SDL_Color fg_col={255,180,200,255};
				SDL_Color bg_col={0,0,0,0};

				//On tente de récupérer les paramètres d'apparence assiciés si disponible.
				if(texts.look.contains(ID)) //1: on regarde si ce texte à un look associé.
				{
					const std::string& look_ID =  texts.look.at(ID); 
	
					if(texts.looks_bank.contains(look_ID)) //2: on vérifie la présence du look demandé.
					{
						const Text_Looking_Params& look = texts.looks_bank.at(look_ID);
							style = look.style; //Les paramètres d'apparence associé à ce texte.
							ptsize = look.ptsize;
							fg_col = look.fg_col;
							bg_col = look.bg_col;
					}
				}
				
				gs::te::Font_Rendering_Type rendering_type=gs::te::BLENDED;
				if(bg_col.r == 0 and bg_col.g == 0 and bg_col.b == 0 and bg_col.a == 0)
					rendering_type=gs::te::SHADED; //S'il n'y a pas de couleur d'arrière plan, alors il n'y a pas lieu de s'en occuper. 
				
				
				font.setFontStyle(style);
				font.setPtsize(ptsize);


				//======================================================================================================



				SDL_Rect out__img_size;
				texture = font.render(text, fg_col, rendering_type, &bg_col, &out__img_size);

				if(not texture)
				{
					std::cout << "!! in 'Game_Text_Textures::loadTexts()', texture loading error : " << font.lastError() << ".\n";
					continue;
				}

				//S'il les choses sont OK, on rajoute la texture chargée.
				m_textures[ID] = texture;
				m_textures_rect[ID] = gs::frect(out__img_size);
			}


			//Restauration du font à l'état reçu. 
			font.setFontStyle(prev_font_style); 
			font.setPtsize(prev_font_ptsize);
		}

		void clear() 
		{
			for(auto  [text, texture] : m_textures)
			{
				if(texture)
					SDL_DestroyTexture(texture);
			}

			m_textures.clear();
			m_textures_rect.clear();
		}

		SDL_Texture* getTexture(const std::string& ID, SDL_FRect* out__img_size)
		{
			if(not m_textures.contains(ID))
				return nullptr; 

			if(out__img_size)
			{
				if(m_textures_rect.contains(ID)) //Par sécurité seulement, (car d'après 'loadTexts()', si l'id est présent dans 'm_textures', il l'est aussi dans 'm_textures_rect'). 
				{
					SDL_FRect rect = m_textures_rect.at(ID);
					out__img_size->w = rect.w;
					out__img_size->h = rect.h;
				}
				else
				{
					out__img_size->w = 100; //Par défaut.
					out__img_size->h = 100;
				}
					
			}

			return m_textures.at(ID);
		}

	protected:
		
		std::unordered_map<std::string, SDL_Texture*> m_textures;	
		std::unordered_map<std::string, SDL_FRect> m_textures_rect;	

};


//===================================================================================

enum Beat_Type{CONSTRUCTION, EXPECTED_RESPONSE};
enum Response_Type{CLICK_DOWN, CLICK_UP};

const int BEAT_STRUCT_Version = 1; //Pour la gestion de la compatibilité dans les sauvegardes.
struct Beat
{
	//version 1
	uint64_t time = 0; // Position temporelle dans le morceau, en µs.
	Beat_Type type = CONSTRUCTION;
	Response_Type resp_type = CLICK_DOWN;

	//Pour garder une trace de son animation.
	gs::Vec2f last_pos_in_anim;
};

struct Beats_Editor 
{
	//Aligne les bâtements sur les beats du métronome (ou de leurs quotients si 'metronome_beat_div > 1'), et retire les doublons.
	static std::vector<Beat> align(std::vector<Beat> beats, double bpm, uint32_t metronome_beat_div = 1) 
	{
		//Toutefois, il est supposé à l'entrée de cette méthode que les beats sont déjà classés par ordre croissant des temps.
		try{

		if(metronome_beat_div == 0) metronome_beat_div = 1; //Sécurité.
		
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


		//Les graduations.
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
		enum Status{NOTHING, PLAYING}; //AUTOPLAYING
		
		Beats_Container() = default;
		~Beats_Container() = default;

		bool setPartition(const std::string& file_path);
		
		void setPartition(const std::vector<Beat>& beats); //Le système ici est statique, c-à-d que l'ensemble des beats d'un morceau sont rajoutés d'un coup et non en flux progressif.
		std::vector<Beat> getPartition() const;

		bool autoPlay(void (*actionCallback)(void)); //Jouer les beats de manière automatique.
		
		void update(double dt); //Arg 'dt' en seconde.

		bool setMusic(const std::string& file_path);

		void setUserFeedbackCallback(void (*userFeedbackCallback)(Beat beat, bool beat_catched, double timing));

		bool userReact();
	

	protected:

		void virtual childClasses_Restarter() {};
		
		std::vector<Beat> m_beats; //Les beats, représentés par leur temps d'arrivée (relatif au au début du morceau), dans l'ordre croissant.
		uint32_t m_curr_beat_idx=0; //Tête de lecture des beats (représente l'indice du prochain beat à dans la lecture).
		
		Status m_status=NOTHING;		

		uint64_t m_timer=0; //Chronomètre, en µs, qui commence au début du morceau (à l'appel de autoPlay()).
		
		uint32_t m_bonus_time_after_end = 5000000; // En µs, un temps pour laisser tourner le jeu des beats après que le dernier beat soit passé. 
		uint32_t m_after_end__timer = 0; // Le timer relié à 'm_bonus_time_after_end'.


		void (*m_actionCallback)(void)=nullptr; //Un callback (optionnel (mais pas vraiment, on y verrait rien sinon)) à exécuter à chaque beat.
		
		sf::Music m_music;
		bool m_music_is_ready = false;	

		
		//==========================================

		double m_min_resp_offset_tolerance = (60000000.0 / 145)*4;//1000000; //La plage temporelle avant l'arrivé d'un beat où une réaction peut être acceptée; en µs. Exemple, si la position temporelle du beat est à 7, et que cette plage est de 2, aucune réation avant la position 5 ne sera comptabilisé pour ce beat.  		
		double m_max_resp_offset_tolerance = 1000000; //La plage temporelle après l'arrivé d'un beat où une réaction peut encore être acceptée; en µs. Exemple, si la position temporelle du beat est à 7, et que cette plage est de 1, aucune réaction après la position 8 ne sera prise, c'est la fin du passage de ce beat. 
		
		//Note: un beat ne peut accepeter qu'une seule réaction, après ça c'est au tour du beat suivant.

		void (*m_userFeedbackCallback)(Beat beat, bool beat_catched, double timing)=nullptr; //Quand les jeu est lancé, cette callback permet de renvoyer les informations relatives au beat en cours (lors qu'il y a click, ou manqué).
		//'beat_catched' : 'true' si l'utilisateur a appuyé/réagi durant la fenêtre temporelle dédié à ce beat, 'false' sinon. 
		//'timing' (à ne pas en tenir compte si 'beat_catched==false') : en µs, l'écart entre le moment de la réaction de l'utilisateur et la position temporelle du beat; peut être positif ou négatif : négatif si la réaction/click est avant l'arrivé du beat, positif si la réaction/click est après l'arrivé du beat. 
		
		uint32_t m_curr_waiting_beat_idx=0; //Représente le beat courant qui attend une réaction (peut être différent du prochain beat à arriver dans la lecture).
		
};




bool Beats_Container::setPartition(const std::string& file_path)
{
	std::ifstream file(file_path, std::ios::in);

	if(not file) 
	{
		std::cout << "!! in 'Beats_Container::setPartition()', can't open file : '" << file_path << "'.\n";
		return false;
	}

	
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


		if(file.eof())
			break;
		else
		if(not file.good())
		{
			std::cout << "!! in 'Beats_Container::setPartition()', some error occured while reading the file.\n";
			return false;
		}
			

		m_beats.push_back(b);		
	}
	
	}catch(std::exception& excep){ std::cout << "Excep in 'Beats_Container::setPartition()' : " << excep.what() << '\n'; return false;}	
	

	setPartition(m_beats); //Par sécurité pour l'ordre croissant des temps.
	
	childClasses_Restarter(); 

	return true;
}

void Beats_Container::setPartition(const std::vector<Beat>& beats)
{
	m_beats = beats;
	
	//Organisation des beats par ordre croissant des temps.
	std::sort(m_beats.begin(), m_beats.end(), [](const Beat& a, const Beat& b){
		return a.time < b.time;
	});

	childClasses_Restarter(); 
}

std::vector<Beat> Beats_Container::getPartition() const {return m_beats;}


bool Beats_Container::autoPlay(void (*actionCallback)(void))
{
	if(m_status != NOTHING) 
		return false; //Pour ne garder qu'une opération à fois.

	m_actionCallback = actionCallback;
	m_timer = 0; //Timer en microseconde.
	m_curr_beat_idx=0;
	m_curr_waiting_beat_idx=0;
	childClasses_Restarter(); 

	if(m_music_is_ready)
	{
		m_music.stop();
		m_music.play();
	}
	
	m_status = PLAYING;
	return true;
}


void Beats_Container::update(double dt) //Arg 'dt' en seconde.
{
	if(m_status==NOTHING) return;

	uint64_t µs_dt = static_cast<uint64_t>(dt*1000000.0); //Conversion : seconde en microseconde.


	if(m_status==PLAYING)
	{

		//Pour éviter d'avoir de grands écarts de timing entre la musique et les beats:
		{
			double music_playing_offset = static_cast<double>(m_music.getPlayingOffset().asMicroseconds());
			double gap = music_playing_offset - static_cast<double>(m_timer);
			
			double max_allowed_gap = 10000; //en µs, arbitrairement choisi.

			if(std::abs(gap) > max_allowed_gap) //Si l'écart entre la musique et le jeu dépasse la limite imposée.
				m_music.setPlayingOffset(sf::microseconds(static_cast<int64_t>(m_timer))); //On réaligne la musique avec le jeu.
		}
		



		if(m_curr_beat_idx >= m_beats.size()) //Si tous les beats sont parcourus.
		{
			//On traite d'abord le délai post fin.
			if(m_after_end__timer < m_bonus_time_after_end)
			{
				m_timer += µs_dt;
				m_after_end__timer += µs_dt;
				return;
			}
	

			//Sinon si ce temps post fin est aussi achevé, on arrête le jeu des beats.

			m_status=NOTHING;

			if(m_music_is_ready)
				m_music.stop();

			m_after_end__timer = 0;

			return;	
		}

		//Sinon
		

		
		const Beat& curr_beat = m_beats.at(m_curr_beat_idx);
		
		//On regarde si son temps est arrivé (ou passé).
		if(curr_beat.time <= m_timer)
		{
			m_curr_beat_idx++;
			
			if(m_actionCallback) m_actionCallback();


			//Règle : S'il y a un beat 'A' qui attend une réaction, tout ayant sa position temporelle déjà lue, et que le beat suivant dans la lecture 'B', vient à être lu, 'B' prend la place de 'A', même si 'A' n'a pas accompli son temps d'après passage. 
			if(m_curr_beat_idx > m_curr_waiting_beat_idx 
			and m_curr_beat_idx-m_curr_waiting_beat_idx > 1)
			{
				//On signale que ce beat n'a pas pu être "capturé" (n'a pas eu de réaction à temps).
				if(m_userFeedbackCallback)
					m_userFeedbackCallback(m_beats.at(m_curr_waiting_beat_idx), false, 0);
				
				//Et l'on passe au suivant.
				m_curr_waiting_beat_idx++;
			}
		}



		//================================================//
		//Les beats "attendent" des réactions, dans une plage de temps imparti.
		
		if(m_curr_waiting_beat_idx < m_beats.size()) //Sécurité
		{
			const Beat& curr_waiting_beat = m_beats.at(m_curr_waiting_beat_idx);

			//On regarde si son temps est accompli (en prenant en compte le délai supplémentaire d'après passage, aka 'm_max_resp_offset_tolerance').	
			if(curr_waiting_beat.time+m_max_resp_offset_tolerance < m_timer)
			{
				//Si l'on arrive dans ce bloc de code, c'est que le beat a accompli son temps, et donc n'a reçu aucune réaction (aucune réaction dans son timing).
				//De ce fait on fait un feedback de cela.

				if(m_userFeedbackCallback)
					m_userFeedbackCallback(curr_waiting_beat, false, 0);
				
				//Et l'on passe au suivant.
				m_curr_waiting_beat_idx++;
			}
		}
		//Ici on ne regarde que si les beats qui attendent sont arrivés à leur terme. La capture des réaction se fait indépendamment via 'Beats_Container::userReact();'. 
	}
	else
		if(m_music_is_ready)
			m_music.stop();
	
	
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


void Beats_Container::setUserFeedbackCallback(void (*userFeedbackCallback)(Beat beat, bool beat_catched, double timing))
{
	m_userFeedbackCallback = userFeedbackCallback;
}


bool Beats_Container::userReact()
{
	if(m_status!=PLAYING) return false;
	
	if(m_curr_waiting_beat_idx >= m_beats.size()) //Sécurité
		return false;

	const Beat& curr_waiting_beat = m_beats.at(m_curr_waiting_beat_idx);
	
	//L'unité de mesure est le µs.	

	double beat_time = static_cast<double>(curr_waiting_beat.time); 
	double time__min_side = beat_time - m_min_resp_offset_tolerance;
	double time__max_side = beat_time + m_max_resp_offset_tolerance;
	
	//On regarde si la réaction a été fait dans la plage d'acceptation du beat.
	if(time__min_side <= m_timer and m_timer <= time__max_side)
	{
		//On informe alors l'utilisateur du la réaction acceptée.
		if(m_userFeedbackCallback)
			m_userFeedbackCallback(curr_waiting_beat, true, static_cast<double>(m_timer) - beat_time);
				
		//Et l'on passe au suivant.
		m_curr_waiting_beat_idx++;

	}

	return true;
}





class Balloon : public Beats_Container
{
	public:
		~Balloon() {if(m_bg) SDL_DestroyTexture(m_bg); if(m_bg_cloud) SDL_DestroyTexture(m_bg_cloud);}
		void showAnim(SDL_Renderer* ren, float dt);

	protected:
		void childClasses_Restarter() override
		{
			m_anim__curr_beats.clear();
			m_cloud_layer1_pos_x = gs::Vec2f(0.f,0.f);
			m_cloud_layer2_pos_x = gs::Vec2f(0.f,0.f);
		}
		//========================================== 
		//Côté animation/visuel

		bool m_anim_is_init = false;

		struct Beat_Anim 
		{
			uint32_t beat_idx;
			int type=0;
		};

		std::vector<Beat_Anim> m_anim__curr_beats; //Les "pointeurs" des beats qui sont actuellement en animation. Ils sont déterminés à partir de 'm_curr_waiting_beat_idx'. Ils sont censé être rangés dans l'ordre croissant. 

		double m_anim__starting_offset =  (60000000.0 / 145)*4;//1000000; //En µs, l'animation d'un beat commence lorsqu'il lui reste 'm_anim__starting_offset' µs ou moins avant l'arrivé de sa position temporelle au chrono.
		gs::Random<std::uniform_real_distribution<float>> m_rd;

		SDL_Texture* m_bg=nullptr;
		std::string  m_bg_path="Ress/bg_1.png";
		SDL_Texture* m_bg_cloud=nullptr;
		std::string  m_bg_cloud_path="Ress/bg_1_nuages.png";
		gs::Vec2f m_cloud_layer1_pos_x; // Pour faire un effet défilement infini (sur l'horizontal, de droite à gauche), on va faire défiler le layer. Cependant il'y aura un vide qui va se créer, d'où de manière simplifiée ici, je vais combler le vide avec la même image (ce qui donne "deux images" (pour ce projet une seule image en plus suffit pour combler le vide)). Note : ce code-ci (classe Balloon) n'est pas spécialement fait pour être générique, mais plutôt tend à répondre aux besoins de CE projet-CI en particulier. 
		gs::Vec2f m_cloud_layer2_pos_x; // Dans ce vecteur le .x est la coo x de l'image de gauche, et le .y est la coo x de l'image de droite. (Pour ces deux layers, les coo sont celle du coin supérieur gauche de l'image).
		

};


void Balloon::showAnim(SDL_Renderer* ren, float dt)
{
	if(not ren) return;

	if(not m_anim_is_init)
	{
		m_rd.setParam(std::uniform_real_distribution<float>::param_type(0.f,100.f));
		
		m_bg = gs::loadImg(ren, m_bg_path);
		m_bg_cloud = gs::loadImg(ren, m_bg_cloud_path);

		if(not m_bg) std::cout << "in Balloon::showAnim() : load background error : " << gs::lastError() << '\n';
		if(not m_bg_cloud) std::cout << "in Balloon::showAnim() : load background cloud error : " << gs::lastError() << '\n';

		m_anim_is_init = true;
	}

	if(m_status!=PLAYING) return;
	
	//On actualise les "pointeurs" vers beats à animer. On prend tous ceux qui sont dans la plage 'm_anim__starting_offset'.
	
	for(int i=static_cast<int>(m_anim__curr_beats.size())-1; i>=0; --i)
	{
		//Le plus ancien ne peut être avant 'm_curr_waiting_beat_idx'. 
		if(m_anim__curr_beats.at(i).beat_idx < m_curr_waiting_beat_idx)
			m_anim__curr_beats.erase(m_anim__curr_beats.begin()+i);
	}

	int last_curr = m_curr_waiting_beat_idx; if(not m_anim__curr_beats.empty()) last_curr = m_anim__curr_beats.back().beat_idx;
	
	if(last_curr >= 0)
	for(int i=last_curr; i<m_beats.size(); ++i) //On rajoute ceux à rajouter.
	{
		const Beat& beat = m_beats.at(i);
		
		if(beat.time < m_timer+m_anim__starting_offset) //Si le beat est dans la plage. 
		{
			if(m_anim__curr_beats.empty() or m_anim__curr_beats.back().beat_idx < i)
			{
				float nb = m_rd.get();
				int type = 0; if(nb > 50) type = 1;
				m_anim__curr_beats.push_back({uint32_t(i), type});
			}
				
		}
		else
			break;

		//Note: sauf erreur, dans 'm_anim__curr_beats', les indices sont dans l'ordre croissant. 
	}
	



	//========= Et puis on anime les différents beats. ==============================
	
	//On commence d'abord par l'arrière plan.
	SDL_RenderTexture(ren, m_bg, nullptr, nullptr);
	{
		//Animation parallax des nuages.
		//Il y'a deux couches de nuages dans la texture, et chacune aura sa vitesse. Dans la texture, elles sont "physiquement" disposées chacune sur une moitié de la texture (moitié supérieure/inférieure). 	
		
		SDL_FRect img_rect; SDL_GetTextureSize(m_bg_cloud, &img_rect.w, &img_rect.h);
		gs::Vec2f cloud_size = {WIN_W, WIN_H/2}; //Pour la destination (affichage).
	
		float layer1_x_speed = 10.f; //Choix arbitraire. En pixel par seconde.

		m_cloud_layer1_pos_x.x -= layer1_x_speed*dt; // C'est sur l'image de gauche qu'on fait évoluer la position. Note : vitesse négative en x pour aller de droite à gauche.
		m_cloud_layer1_pos_x.y = m_cloud_layer1_pos_x.x + cloud_size.x; //En suite on fait suivre celle de droite. Note : on défini la postion de l'image de droite de manière à ce que cette dernière soit juxtaposée à droite de l'image de gauche.

		if(m_cloud_layer1_pos_x.x + cloud_size.x <= 0) // Si toute l'image de gauche est passé hors écran, par le côté gauche de l'écran.
			std::swap(m_cloud_layer1_pos_x.x, m_cloud_layer1_pos_x.y); //On interchange les rôles entre l'image de gauche et celle de droite.

		SDL_FRect cloud1_dest_left = {m_cloud_layer1_pos_x.x, 0, WIN_W, WIN_H/2};		
		SDL_FRect cloud1_dest_right = {m_cloud_layer1_pos_x.y, 0, WIN_W, WIN_H/2};
		
		SDL_FRect lay1_src = {0.f, 0.f, img_rect.w, img_rect.h/2.f};

		SDL_RenderTexture(ren, m_bg_cloud, &lay1_src, &cloud1_dest_left);
		SDL_RenderTexture(ren, m_bg_cloud, &lay1_src, &cloud1_dest_right);
		


		//Même démarche pour la deuxième couche.

		float layer2_x_speed = 7.f; //Choix arbitraire. En pixel par seconde.

		m_cloud_layer2_pos_x.x -= layer2_x_speed*dt; // C'est sur l'image de gauche qu'on fait évoluer la position. Note : vitesse négative en x pour aller de droite à gauche.
		m_cloud_layer2_pos_x.y = m_cloud_layer2_pos_x.x + cloud_size.x; //En suite on fait suivre celle de droite. Note : on défini la postion de l'image de droite de manière à ce que cette dernière soit juxtaposée à droite de l'image de gauche.

		if(m_cloud_layer2_pos_x.x + cloud_size.x <= 0) // Si toute l'image de gauche est passé hors écran, par le côté gauche de l'écran.
			std::swap(m_cloud_layer2_pos_x.x, m_cloud_layer2_pos_x.y); //On interchange les rôles entre l'image de gauche et celle de droite.

		SDL_FRect cloud2_dest_left = {m_cloud_layer2_pos_x.x, 0, WIN_W, WIN_H/2};		
		SDL_FRect cloud2_dest_right = {m_cloud_layer2_pos_x.y, 0, WIN_W, WIN_H/2};
		
		SDL_FRect lay2_src = {0.f, img_rect.h/2.f, img_rect.w, img_rect.h/2.f};

		SDL_RenderTexture(ren, m_bg_cloud, &lay2_src, &cloud2_dest_left);
		SDL_RenderTexture(ren, m_bg_cloud, &lay2_src, &cloud2_dest_right);
	}
	
	
	
	

	gs::Vec2f start_pos = {100,250};
	gs::Vec2f end_pos = {WIN_W/2, WIN_H/2};
	
	for(float i=-1; i<=1; ++i)
	{
		//gs::drawLine(ren, end_pos-gs::Vec2f{40.f,i}, end_pos+gs::Vec2f{40.f,-i}, {0,0,0,255});
		gs::drawLine(ren, end_pos-gs::Vec2f{i,400.f}, end_pos+gs::Vec2f{-i,400.f}, {255,220,0,255});
	}	

	for(auto beat_anim : m_anim__curr_beats)
	{
		uint32_t beat_idx = beat_anim.beat_idx;

		if(beat_idx < 0 or beat_idx >= m_beats.size()) //Sécurité
		{
			std::cout << "debug: 'beat_idx' invalid : "<< beat_idx << ", m_beats.size() : " << m_beats.size() << '\n'; 
			continue;
		}

		Beat& beat = m_beats.at(beat_idx);
		
		#if 0
		    //Petite animation simple, linéaire, d'un déplacement à trajectoire rectiligne d'un point A vers un point B.
	
			//le pourcentage de la progession du beat vers son objectif, en fonction du temps. 0% = est au début, 100% = est à la fin. 
			float progr =  1.0 - (static_cast<double>(beat.time) - static_cast<double>(m_timer))/ (m_anim__starting_offset);
	
			gs::Vec2f curr_pos = start_pos + (end_pos - start_pos)*progr;
			
			gs::drawCircle(ren, curr_pos.to<int>(), 38, {255,50,150,150}, true);
		#endif

		//le pourcentage de la progession du beat vers son objectif, en fonction du temps. 0% = est au début, 100% = est à la fin. 
		float progr =  1.0 - (static_cast<double>(beat.time) - static_cast<double>(m_timer))/ (m_anim__starting_offset);
		
		float begin_x = 0;
		float begin_y = 0;
		float target_x = 0;
		float target_y = 0;

		float fx = 0;
		float fy = 0;

		float fymin = 0;
		float fymax = 0;
		
		if(beat_anim.type == 0)
		{
			begin_x = -5;
			begin_y = 0;
			target_x = WIN_W/2;
			target_y = float(WIN_H)*0.7;
			
			fx = (3.1415926f/2.f) * progr;
			fy = std::sin(fx); //Utilisation de la fonction sinus pour la trajectoire.

			fymin = -1;
			fymax = 1;
		}
		else
		{
			begin_x = -5;
			begin_y = 100;
			target_x = WIN_W/2;
			target_y = float(WIN_H)*0.7;
			
			fx = gs::changeRange(progr, 0,1, -2,0); 
			fy = fx*fx; //Utilisation de la fonction carré pour la trajectoire.

			fymin = 0;
			fymax = 4;
		}


		gs::Vec2f pos = {begin_x+(target_x-begin_x)*progr, begin_y+float(gs::changeRange(fy,fymin,fymax,0, target_y-begin_y))};
			
		pos.y = float(WIN_H) - pos.y; //Inversion de l'axe des ordonnées. 
		
		float min_radius = 5.f;
		float max_radius = 38.f;
		float radius = gs::changeRange(progr, 0,1, min_radius, max_radius);	
		int thickness = gs::changeRange(progr, 0,1, 1.02, 5);		

		gs::drawCircle(ren, pos.to<int>(), int(radius), {255,50,150,255}, true);
		gs::drawCircleThickness(ren, pos.to<int>(), int(radius), thickness, {0,10,10,255}, true);

		beat.last_pos_in_anim = pos;
		
	}
}




//===================================================================================


gs::Text_Line util_ln;
gs::Rendered_Str_Bank str_bank;
gs::Font font;

gs::Mouse_Tracker mouse_left;
gs::Mouse_Tracker mouse_right;
gs::Input input;
gs::Random<std::normal_distribution<float>> normal_rand;

Balloon beats;
Beats_Viewer b_viewer;


sf::SoundBuffer balloon_explo_sound_buff;
sf::Sound balloon_explo_sound1(balloon_explo_sound_buff);
sf::Sound balloon_explo_sound2(balloon_explo_sound_buff);

Game_Text TXT;
Game_Text_Textures txt_imgs;

SDL_Texture* pic_img=nullptr;
SDL_Texture* balloon_frags_img=nullptr;

std::vector<Animation> anims;

enum Playing_Mark{NONE, TOO_EARLY, GOOD, NICE, TOO_LATE, MISSED};
Playing_Mark last_mark; //Concerne les beats lors du jeu. 


void Init(SDL_Window* win, SDL_Renderer* ren);void InputGestion(bool& boolvar);void Update(float dt);
void Draw(SDL_Renderer* ren, float dt);
void beatCallback();
void playingBeatsStatesAlerter(Beat beat, bool beat_catched, double timing);
int addAnim(gs::Vec2f origin_pos, std::string name);

int main() 
{
	gs::Game game; 
	game.InitSDLEngine(WIN_W, WIN_H, "Rhythm Balloon");
	if(not game.isGameInit())
	{
		std::cout << "game.InitSDLEngine() failled\n";
		return 1;
	}

	SDL_Window* win=game.win();	SDL_Renderer* ren=game.ren();
	ThisGame = &game;

	Init(win, ren);
	
	
	
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
	SDL_SetRenderDrawColor(ren, 120u, 42u, 20u, 255u);

	mouse_left.setTargetBtn(true);
	mouse_right.setTargetBtn(false);

	normal_rand.initForFormattedGet(normal_rand.getParam().mean(), normal_rand.getParam().stddev(), "normal");

	//===============================================
	std::string font_path = "C:/prog_utils/Aru_fonts/Carter_One/CarterOne-Regular.ttf";
	if(not util_ln.initAll(ren, font, str_bank, font_path.c_str(), 35, gs::u32RGBA(20,24,15,255)))
	{
		std::cout << "Font init failled, font path : " <<font_path<<"\n";
	}
	else
	{
		txt_imgs.loadTexts(font, TXT);
	}

	beats.setUserFeedbackCallback(playingBeatsStatesAlerter);
	beats.setMusic("Ress/Audio/free/Rhythm_base_audio_test_1_145bpm.mp3");
	beats.setPartition("Beats_rhythm_test1_145bpm.txt");
	beats.setPartition(Beats_Editor::align(beats.getPartition(), 145, 1));
	
	bool success = balloon_explo_sound_buff.loadFromFile("Ress/Audio/may_be_free/balloon-explosion-single-voiced-close.wav");// balloon-explosion-single-sharp-voiced.wav");
	
	b_viewer.setPartition(beats.getPartition());
	b_viewer.m_area = SDL_FRect(50,20,700,80);


	pic_img = gs::loadImg(ren, "Ress/pic.png");
	balloon_frags_img = gs::loadImg(ren, "Ress/balloon_frags.png");
	
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
		beats.autoPlay(nullptr);
		b_viewer.play(145);
	}
	
	if(mouse_right.clickedDown())
		beats.userReact();
	
	
		
	beats.update(dt);
	


	for(int i=static_cast<int>(anims.size())-1; i>=0; --i)
	{
		if(not anims.at(i).isPlaying())
		{
			anims.erase(anims.begin()+i);
			continue;
		}

		
		anims.at(i).update(dt);;
	}

	
}



void Draw(SDL_Renderer* ren, float dt)
{
	
	beats.showAnim(ren, dt);
	
	
	for(Animation& anim : anims)
		anim.show(ren);
	
}



void playingBeatsStatesAlerter(Beat beat, bool beat_catched, double timing)
{
	short timing_sign = gs::sign(timing);
	timing = abs(timing);

	
	if(beat_catched)
	{
		if(timing < 50000/2)
		{
			last_mark = NICE; 
			addAnim({float(WIN_W)*0.5f, float(WIN_H)*0.5f}, "#nice");
		}
		else 
		if(timing < 100000)
		{
			last_mark = GOOD; 
			addAnim({float(WIN_W)*0.5f, float(WIN_H)*0.7f}, "#good");
		}	
		else
		{
			std::string anim_name = "#too_early";

			if(timing_sign > 0)
			{
				last_mark = TOO_LATE; 
				anim_name = "#too_late";
			}
			else
				last_mark = TOO_EARLY;
		
			

			gs::Vec2f anim_origin = {normal_rand.get(beat.last_pos_in_anim.x, 200.f), 
									normal_rand.get(beat.last_pos_in_anim.y, 150.f)};
			
			float min_x=float(WIN_W)*0.2f, max_x=float(WIN_W)*0.8f; 
			float min_y=float(WIN_H)*0.2f, max_y=float(WIN_H)*0.8f;

			if(anim_origin.x<min_x) anim_origin.x=min_x; else if(anim_origin.x>max_x) anim_origin.x=max_x;
			if(anim_origin.y<min_y) anim_origin.y=min_y; else if(anim_origin.y>max_y) anim_origin.y=max_y;

			addAnim(anim_origin, anim_name);
		}

		addAnim(beat.last_pos_in_anim, "balloon_boom"); 
		addAnim(beat.last_pos_in_anim, "pic"); 
		

		if(balloon_explo_sound1.getStatus() != sf::SoundSource::Status::Playing) 
			balloon_explo_sound1.play();
		else
		{	
			if(balloon_explo_sound2.getStatus() != sf::SoundSource::Status::Playing)
				balloon_explo_sound2.play(); 
			else
			{
				balloon_explo_sound1.stop();
				balloon_explo_sound1.play();
			}
		}
		
	}
	else 
		last_mark = MISSED;
}



int addAnim(gs::Vec2f origin_pos, std::string name)
{
	gs::Vec2f n = {0.f,0.f};

	if(name == "pic")
	{
		anims.emplace_back();
		Animation& anim = anims.back();
			anim.addItem(true, true, true);
				anim.setItemProperties(0, pic_img, {100.f,200.f});
				anim.addAnimStep(0, 0.03, {8,-16}, {0,0}, 32,32, {1,1},{0.57,0.57});
				anim.addAnimStep(0, 0.8, n, {100,-80}, 32,32);
			anim.setAnimOriginPos(origin_pos);
			anim.startAnimation();
	}
	else 
	if(name == "balloon_boom")
	{
		float size = 150; //La taille source (d'une fragment), et note : ici w == h.

		anims.emplace_back();
		Animation& anim = anims.back();
			anim.addItem(true, true, true);
				anim.setItemProperties(0, balloon_frags_img, {50.f,50.f}, {0+3,0+3,size-6,size-6});
				//anim.addAnimStep(0, 0.02, {-19+10,-18+18}, {-19+10,-18+18});
				anim.addAnimStep(0, 0.5, {-19+10,-18+18}, {-38,-36}, 0,-9, {0.5,0.5},{1.f,1.f});
				
			anim.addItem(true, true, true);
				anim.setItemProperties(1, balloon_frags_img, {50.f,50.f}, {size+3,0+3,size-6,size-6});
				//anim.addAnimStep(1, 0.02, {5-10,-15+10}, {5-10,-15+10});
				anim.addAnimStep(1, 0.5, {5-10,-15+10}, {9,-28}, 0,8, {0.5,0.5},{1.f,1.f});
			
			anim.addItem(true, true, true);
				anim.setItemProperties(2, balloon_frags_img, {50.f,50.f}, {size*2+3,0+3,size-6,size-6});
				//anim.addAnimStep(2, 0.02, {21-10,-11+10}, {21-10,-11+10});
				anim.addAnimStep(2, 0.5, {21-10,-11+10}, {42,-22}, 0,13, {0.5,0.5},{1.f,1.f});
				
			anim.addItem(true, true, true);
				anim.setItemProperties(3, balloon_frags_img, {50.f,50.f}, {0+3,size+3,size-6,size-6});
				//anim.addAnimStep(3, 0.02, {-18+10,20-10}, {-18+10,20-10});
				anim.addAnimStep(3, 0.5, {-18+10,20-10}, {-36,40}, 0,-14, {0.5,0.5},{1.f,1.f});
				
			anim.addItem(true, true, true);
				anim.setItemProperties(4, balloon_frags_img, {50.f,50.f}, {size+3,size+3,size-6,size-6});
				//anim.addAnimStep(4, 0.02, {19-10,21-10}, {19-10,21-10});
				anim.addAnimStep(4, 0.5, {19-10,21-10}, {38,42}, 0,11, {0.5,0.5},{1.f,1.f});
				
	

			anim.setAnimOriginPos(origin_pos);
			anim.startAnimation();
	}
	else
	if(name == "#too_early" or name == "#too_late")
	{
		SDL_FRect r;
		SDL_Texture* img = txt_imgs.getTexture(name, &r);

		anims.emplace_back(); //Rajout d'une animation.
		Animation& anim = anims.back(); //On la récupère afin de la modifier. 
			anim.addItem(true, true, true); //On rajoute un objet/élément dans cette animation (que l'on va localiser par son indice (qui est par ordre d'arrivé)).
				anim.setItemProperties(0, img, {r.w,r.h}); //On associe une image à cet élément (élément d'indice 0).
				anim.addAnimStep(0, 0.25f, {0.f,0.f},{0.f,0.f},  0.f,25); //Ensuite, on défini les différentes étapes d'animation de cet élément. 
				anim.addAnimStep(0, 0.25f, {0.f,0.f},{0.f,0.f},  0.f,0);
				anim.addAnimStep(0, 0.25f, {0.f,0.f},{0.f,0.f},  0.f,-25);
				anim.addAnimStep(0, 0.25f, {0.f,0.f},{0.f,0.f},  0.f,0);
				anim.addAnimStep(0, 0.3f, {0.f,0.f},{0.f,-30.f},  0.f,0, {1.f,1.f},{2.5f,1.5f});
			anim.setAnimOriginPos(origin_pos); //On règle aussi le repère de l'animation.
			anim.startAnimation(); //Enfin on lance cet animation.
	}
	else
	if(name == "#good")
	{
		SDL_FRect r;
		SDL_Texture* img = txt_imgs.getTexture(name, &r);

		anims.emplace_back(); //Rajout d'une animation.
		Animation& anim = anims.back(); //On la récupère afin de la modifier. 
			anim.addItem(true, true, true); //On rajoute un objet/élément dans cette animation (que l'on va localiser par son indice (qui est par ordre d'arrivé)).
				anim.setItemProperties(0, img, {r.w,r.h}); //On associe une image à cet élément (élément d'indice 0).
				anim.addAnimStep(0, 1.f, {0.f,0.f},{0.f,-20.f},  0.f,0.f, {1.8f,1.8f},{0.0f,0.0f}); //Ensuite, on défini les différentes étapes d'animation de cet élément. 
			anim.setAnimOriginPos(origin_pos); //On règle aussi le repère de l'animation.
			anim.startAnimation(); //Enfin on lance cet animation.
	}
	else
	if(name == "#nice")
	{
		SDL_FRect r;
		SDL_Texture* img = txt_imgs.getTexture(name, &r);

		anims.emplace_back(); //Rajout d'une animation.
		Animation& anim = anims.back(); //On la récupère afin de la modifier. 
			anim.addItem(true, true, true); //On rajoute un objet/élément dans cette animation (que l'on va localiser par son indice (qui est par ordre d'arrivé)).
				anim.setItemProperties(0, img, {r.w,r.h}); //On associe une image à cet élément (élément d'indice 0).
				anim.addAnimStep(0, 0.1f, {0.f,0.f},{0.f,0.f},  -25.f,10.f, {0.f,0.f},{1.3f,1.3f}); //Ensuite, on défini les différentes étapes d'animation de cet élément. 
				anim.addAnimStep(0, 0.3f, {0.f,0.f},{0.f,0.f},  0.f,-5.f, {0.f,0.f},{0.8f,0.8f});
				anim.addAnimStep(0, 0.3f, {0.f,0.f},{0.f,0.f},  0.f,0.f, {0.f,0.f},{1.f,1.f});
			anim.setAnimOriginPos(origin_pos); //On règle aussi le repère de l'animation.
			anim.startAnimation(); //Enfin on lance cet animation.
	}

	return 0;
}














