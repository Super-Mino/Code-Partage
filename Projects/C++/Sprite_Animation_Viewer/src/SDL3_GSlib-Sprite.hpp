#ifndef _SDL3_GSlib_SPRITE_HPP_
#define _SDL3_GSlib_SPRITE_HPP_

#include "SDL3_GSlib.hpp"

namespace gs
{

class Sprite_Sheet
{
	public :

		//Le principe:
		
		//La feuille de sprite sera divisée en plusieurs cases (une case = une frame)
		//selon les dimensions données pour une case.
		
		//Les cases seront numérotées de gauche à droite, de haut en bas. 
		//		Note: Les cases dont le découpage ne donne pas la taille demandée, 
		//		(dû au fait que les dimensions de l'image donnée
		//		ne soient pas multiples des dimensions d'une case), ne seront pas prises en compte.
		//		D'où l'intérêt de faire des sprite-sheets dont la largeur est multiple de la largeur 
		//		d'une case (d'une frame), et dont la hauteur est multiple de la hauteur d'une case.
		//		Ces cases non comptabilisées sont celles aux extrémités droites et basses.
		
		
		//Pour déterminer une animation, l'utilisateur devra définir une liste de cases dont l'ordre 
		//des cases définit l'ordre de passage de ces derniers. 
		//		Note: cet ordre est libre, par exemple on peut avoir : 4,5,6,2,5,2,5,7,
		//		alors l'animation se fera en commençant par la case d'indice 4, puis d'indice 5,
		//		6, 2, et ainsi de suite.
		//		Note: une case peut revenir plusieurs fois dans la même animation.
		//		Note: les indices commencent à partir de 0.
		  
		
		//Information:
		//Les SDL_Texture* chargées depuis un chemin sont libérées par le destructeur de cette class,
		//mais pour celles reçues déjà chargées, la libération n'est pas à la charge du destructeur de cette class. 
		

		Sprite_Sheet()=default;
		~Sprite_Sheet();
		
		void unload();
		
		bool load(SDL_Renderer* ren, const std::string& img_path, Vec2us frame_size, float frame_delay=0.18f);
		bool load(SDL_Texture* texture, Vec2us frame_size, float frame_delay=0.18f);
		
		bool changeFrameSize(Vec2us new_frame_size);
		
		bool setAnim(std::vector<int> frame_idxs, float frame_delay=-1.f); //'frame_delay' est ici facultatif et n'est donc pris en compte que si est strictement positif. 
		
		void update(float dt);
		
		bool start(bool once=false);
		bool stop();
		bool resume();
		
		
		bool drawCurrentFrame(SDL_Renderer* ren, const SDL_FRect* dest, const float& angle_rad=0.f, const SDL_FlipMode& flip=SDL_FLIP_NONE); //Dans la cible de rendu courante.
		bool drawCurrentFrame(SDL_Renderer* ren, const SDL_FRect& dest, const float& angle_rad=0.f, const SDL_FlipMode& flip=SDL_FLIP_NONE); //Dans la cible de rendu courante.
		
		bool setFrameDelay(float sec);
		
		SDL_Texture* getTexture() {return m_texture;}
		
		bool isLoaded() const {return m_is_loaded;}
		
		Vec2f getTextureSize() const;
		
		Vec2f getFrameSize() const {return m_frame_size;}
		
		float getFrameDelay() const {return m_frame_delay;}
		
		Vec2i getFramesCount() const {return m_frames_count;}
		
		bool animIsPlaying() const {return m_anim_is_playing;}
		
		std::vector<int> getAnim() const {return m_anim_frames;}


	protected :
	
		
		bool m_is_loaded=false;
		Vec2f m_frame_size;
		float m_frame_delay=0.18f;
		
		Vec2i m_frames_count={0,0}; //Le nombre de cases : x : nombre de colonnes, y : nombre de lignes, de la sprite-sheet courante.
		
		bool m_anim_is_playing = false;
		int m_curr_anim_pos=0; //Pointe dans 'm_anim_frames'.
		std::vector<int> m_anim_frames; //Les indices des frames à animer.
		float m_timer=0.f; //Un timer à rebours ayant la durée d'une frame.
		bool m_once=false; //Détermine si une fois lancée, l'animation ne se joue qu'une seule fois.
		
		SDL_Texture* m_texture=nullptr;
		bool m_texture_is_from_inside=true;
			bool m_load_with_path__is_called=false; //Variable d'aide pour déterminer l'origine de la texture lors d'un chargement.
			bool m_changeFrameSize__is_called=false;
};



} //namespace gs


#endif // _SDL3_GSlib_SPRITE_HPP_















