#ifndef ANIM_H__
#define ANIM_H__

#include <SDL3_GSlib.hpp>


class Animation
{

	// Une Animation est définie dans son propre repère, dont l'origine peut être placé arbitrairement dans le rendu.
	// Les coordonnées des images d'un Animation sont des coordonnées centrales (non pas du coin supérieur gauche).
	// Un Animation peut être composé de plusieurs items (qui sont indépendants entre eux).
	// Un item peut avoir plusieurs séquences d'animation. 
	// Une séquence comporte une valeur de début et une valeur de fin pour les caractéristiques suivantes : position, rotation, scale.	
	// L'animation interpole les valeurs en fonction de l'avancement du timer. 

	public:
		Animation()=default;
		~Animation()=default;
		
		struct Anim_Step //Une étape d'animation (une séquence).
		{
			float duration=1.f; //En seconde.
			gs::Vec2f begin_pos, end_pos; //En pixel.
			float begin_rot=0.f, end_rot=0.f; //En degré.
			gs::Vec2f begin_scale={1.f,1.f}, end_scale={1.f,1.f};

			void getCurrState(float progr_perc, gs::Vec2f& out__pos, float& out__rot, gs::Vec2f& out__scale)
			{
				out__pos = begin_pos + (end_pos - begin_pos) * progr_perc;
				out__rot = begin_rot + (end_rot - begin_rot) * progr_perc;
				out__scale = begin_scale + (end_scale - begin_scale) * progr_perc;
			}	
		};

		struct Anim_Item
		{
			enum Anim_State{NONE, PLAYING, PAUSED};

			Anim_State m_state=NONE;
			int m_curr_step_idx = -1;
			std::vector<Anim_Step> m_steps;
			float m_timer=0.f; //Se réinitialise à chaque début de séquence. 
			
			SDL_Texture* m_img=nullptr;
			SDL_FRect m_src_rect;
			bool m_show_full_img = true; //Par défaut, pour ne pas prendre en compte m_src_rect.
			gs::Vec2f m_dest_size;
			
			gs::Vec2f m_last_pos;
			float m_last_rot=0.f; //En degré.
			gs::Vec2f m_last_scale={1.f,1.f};

			bool m_string_pos=true, m_string_rot=true, m_string_scale=true; //Veut dire que les valeurs finales d'une séquence sont utilisées comme valeurs de début de la séquence suivante. 

			void start()
			{
				m_state = PLAYING;
				m_curr_step_idx = 0;
				m_timer = 0.f;
			}

			void show(SDL_Renderer* ren, gs::Vec2f plan_origin)
			{
				if(not ren or not m_img) return;

				if(m_state != PLAYING) return;

				gs::Vec2f ren_pos = {plan_origin.x+m_last_pos.x, plan_origin.y+m_last_pos.y};

				gs::Vec2f scaled_size = m_dest_size * m_last_scale;

				SDL_FRect dest = {ren_pos.x-scaled_size.x/2.f, ren_pos.y-scaled_size.y/2.f, scaled_size.x, scaled_size.y};

				SDL_RenderTextureRotated(ren, m_img, (m_show_full_img ? nullptr : &m_src_rect), &dest, static_cast<double>(m_last_rot), nullptr, SDL_FLIP_NONE);
			}

			void update(float dt)
			{
				if(m_state == NONE or m_state == PAUSED) return;
				
				if(m_state == PLAYING)
				{
					if(m_curr_step_idx < 0 or m_curr_step_idx >= m_steps.size()) //Si l'indice de l'étape courante n'est pas invalide.
					{
						m_state = NONE;
						return;
					}
	
					Anim_Step& curr_step = m_steps.at(m_curr_step_idx);

					if(m_timer > curr_step.duration) //Si le temps de l'étape courante est accompli, on passe à l'étape/séquence suivante (s'il y en a).
					{
						m_curr_step_idx++;  
						
						if(m_curr_step_idx >= m_steps.size()) 
						{
							m_state = NONE;
						}
						else
						{
							Anim_Step& next_step = m_steps.at(m_curr_step_idx);
							
							if(m_string_pos) next_step.begin_pos = m_last_pos;
							if(m_string_rot) next_step.begin_rot = m_last_rot;
							if(m_string_scale) next_step.begin_scale = m_last_scale;
						}

						m_timer = 0.f;

						return;
					}
			
					float progr_perc = m_timer / curr_step.duration;

					curr_step.getCurrState(progr_perc, m_last_pos, m_last_rot, m_last_scale);

			
					m_timer += dt;
				}
			}

		};


		void addItem(bool string_pos=true, bool string_rot=true, bool string_scale=true)
		{
			m_items.emplace_back();
			m_items.back().m_string_pos = string_pos;
			m_items.back().m_string_rot = string_rot;
			m_items.back().m_string_scale = string_scale;
		}

		bool setItemProperties(int item_idx, SDL_Texture* img, gs::Vec2f dest_size, SDL_FRect src_rect={-1.f,-1.f,-1.f,-1.f})
		{
			//Note: la gestion de la libération de la mémoire de l'image n'est pas gérée ici, c'est à faire soi-même (ce n'est pas ici le rôle de l'animation). 

			if(item_idx < 0 or item_idx >= m_items.size()) return false;			
			
			Anim_Item& item = m_items.at(item_idx);
			item.m_img = img;
			item.m_dest_size = dest_size;
			if(not (src_rect.x <0.f and src_rect.y <0.f and src_rect.w <0.f and src_rect.h <0.f))
			{
				item.m_src_rect = src_rect;
				item.m_show_full_img = false; //Veut dire qu'on affiche pas toute l'image par défaut, mais va utiliser m_src_rect pour déterminer la portion de l'image à afficher. 
			}
			else
				item.m_show_full_img = true;
			
			return true;
		}

		bool addAnimStep(int item_idx, float duration, gs::Vec2f begin_pos, gs::Vec2f end_pos, float begin_rot=0.f, float end_rot=0.f, gs::Vec2f begin_scale={1.f,1.f}, gs::Vec2f end_scale={1.f,1.f})
		{
			if(item_idx < 0 or item_idx >= m_items.size()) return false;			
			
			Anim_Item& item = m_items.at(item_idx);
			Anim_Step step;
			step.duration = duration;
			step.begin_pos = begin_pos; step.end_pos = end_pos;
			step.begin_rot = begin_rot; step.end_rot = end_rot;
			step.begin_scale = begin_scale; step.end_scale = end_scale;
			item.m_steps.push_back(step);

			return true;
		}

		void startAnimation()
		{
			for(Anim_Item& item : m_items)
				item.start();
			
		}

		void update(float dt)
		{
			for(Anim_Item& item : m_items)
				item.update(dt);
		}


		void show(SDL_Renderer* ren)
		{
			for(Anim_Item& item : m_items)
				item.show(ren, m_anim_origin_pos);
		}

		void setAnimOriginPos(gs::Vec2f anim_origin_pos) {m_anim_origin_pos = anim_origin_pos;}
	
		bool isPlaying()
		{
			for(Anim_Item& item : m_items)
				if(item.m_state == Anim_Item::PLAYING)
					return true;
			return false;
		}

	protected: 
		std::vector<Anim_Item> m_items;
		gs::Vec2f m_anim_origin_pos={0.f,0.f};
};








#endif // ANIM_H__

