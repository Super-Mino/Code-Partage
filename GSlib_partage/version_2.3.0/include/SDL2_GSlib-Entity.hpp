#ifndef _SDL2_GSlib_ENTITY_HPP_
#define _SDL2_GSlib_ENTITY_HPP_
//version modifiée le 14/07/23 (à moins d'oubli de modifier cette ligne).

	#include "SDL2_GSlib.hpp"


namespace sm /*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*/
{

namespace en
{
	enum Corner{TOP_LEFT, TOP_RIGHT, DOWN_LEFT, DOWN_RIGHT, MIDDLE};
}


struct Input
{
	bool 	up=false, 		down=false, 	left=false, 	right=false, 
			space=false, 	a=false, 		b=false, 		y=false, 
			x=false, 		r=false, 		l=false, 		zr=false, 
			zl=false, 		start=false;
}; 


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------



struct Hitbox   
{
	SDL_Rect main_rect;

	/* //Ces parties sont masqué car inutilisées pour l'instant.
	SDL_Rect main_rect, rect1, rect2, rect3, rect4; //hg, hd, bg, bd; 
	short rect_nb=5;



		short getRectNb() {return rect_nb;} 

		short getRectX(short nb) {if(nb<=0) nb = 1; if(nb>=5) nb = 4; 
			if(nb == 1) return rect1.x; if(nb == 2) return rect2.x;  if(nb == 3) return rect3.x; if(nb == 4) return rect4.x;}; 

		short getRectY(short nb) {if(nb<=0) nb = 1; if(nb>=5) nb = 4; 
			if(nb == 1) return rect1.y; if(nb == 2) return rect2.y;  if(nb ==3 ) return rect3.y; if(nb ==4 ) return rect4.y;};

		short getRectW(short nb) {if(nb<=0) nb = 1; if(nb>=5) nb = 4; 
			if(nb == 1) return rect1.w; if(nb == 2) return rect2.w;  if(nb ==3 ) return rect3.w; if(nb ==4 ) return rect4.w;};

		short getRectH(short nb) {if(nb<=0) nb = 1; if(nb>=5) nb = 4; 
			if(nb == 1) return rect1.h; if(nb == 2) return rect2.h;  if(nb ==3 ) return rect3.h; if(nb ==4 ) return rect4.h;};




		void setRect(short nb, std::string var, int var1)
			{ 
				if(nb == 1)
				{
					if(var == "x")     {rect1.x = var1;}
					else if(var == "y"){rect1.y = var1;}
					else if(var == "w"){rect1.w = var1;}
					else if(var == "h"){rect1.h = var1;}
				}
				if(nb == 2)
				{
					if(var == "x")     {rect2.x = var1;}
					else if(var == "y"){rect2.y = var1;}
					else if(var == "w"){rect2.w = var1;}
					else if(var == "h"){rect2.h = var1;}
				}
				if(nb == 3)
				{
					if(var == "x")     {rect3.x = var1;}
					else if(var == "y"){rect3.y = var1;}
					else if(var == "w"){rect3.w = var1;}
					else if(var == "h"){rect3.h = var1;}
				}
				if(nb == 4)
				{
					if(var == "x")     {rect4.x = var1;}
					else if(var == "y"){rect4.y = var1;}
					else if(var == "w"){rect4.w = var1;}
					else if(var == "h"){rect4.h = var1;}
				}
			}

		void setMainRect(std::string var, int var1)
			{
				if(var == "x")     {main_rect.x = var1;}
				else if(var == "y"){main_rect.y = var1;}
				else if(var == "w"){main_rect.w = var1;}
				else if(var == "h"){main_rect.h = var1;}
			}
	


	void setRectNb(short nb)
		{ if(nb > 4) nb = 4; if(nb<1) nb = 1; rect_nb = nb;}

	void setRect(short nb, int x, int y, int w, int h)
		{	if(nb <= 1) {rect1.x = x; rect1.y = y; rect1.w = w; rect1.h = h;}
			if(nb == 2)	{rect2.x = x; rect2.y = y; rect2.w = w; rect2.h = h;}
			if(nb == 3) {rect3.x = x; rect3.y = y; rect3.w = w; rect3.h = h;}
			if(nb >= 4) {rect4.x = x; rect4.y = y; rect4.w = w; rect4.h = h;}
		};

	*/

	void setMainRect(int x, int y, int w, int h)
		{ main_rect.x = x; main_rect.y = y; main_rect.h = h; main_rect.w = w;}

	/*	
	void drawRects(Game *g) 
		{
			if(not g)
			{
				sm::lastError("||Warning|| in 'sm::Hitbox::drawRects()', given sm::Game is nullptr. ");
				return;
			}
			SDL_Rect rects[] = {main_rect, rect1, rect2, rect3, rect4};
			if(SDL_RenderDrawRects(g->ren(), rects, rect_nb ) < 0) std::cout << SDL_GetError(); 
		}*/
	void drawMainRect(Game *g, bool fill)
		{
			if(not g)
			{
				sm::lastError("||Warning|| in 'sm::Hitbox::drawMainRect()', given sm::Game is nullptr. ");
				return;
			}

			if(fill)
				SDL_RenderFillRect(g->ren(), &main_rect);
			else
				SDL_RenderDrawRect(g->ren(), &main_rect);
		}

	int& x() {return main_rect.x;}
	int& y() {return main_rect.y;}
	int& w() {return main_rect.w;}
	int& h() {return main_rect.h;}
	
}; 



//_________________________________________________________________________________
//_________________________________________________________________________________
//----------------------------------------------------------------------------------------


class GameObject
{
	public:

		GameObject() = default;
		
		~GameObject() 
		{
			if(m_destroy_SDL_ptrs_in_destructor)
				if(image != nullptr) SDL_DestroyTexture(image);
			else
				::sm::addToClearAtTheEnd(image);
		}

		
		SDL_Texture* getTexture() {return image;}
		
		SDL_Rect* getDest() {return &dest;}
		
		SDL_Rect* getSrc() {return &src;}

		void setDestroyTextureInTheDestructor(bool state) 
		{m_destroy_SDL_ptrs_in_destructor = state;}

		bool getDestroyTextureInTheDestructor()
		{return m_destroy_SDL_ptrs_in_destructor; }

		
	protected : 
		SDL_Rect dest, src;
		SDL_Texture *image{nullptr};
		bool m_destroy_SDL_ptrs_in_destructor=false;

};


//----------------------------------------------------------------------------------------
//_________________________________________________________________________________
//_________________________________________________________________________________



class Body : public GameObject
{
	public : 

		Body();
		

		~Body();


		//seters
		void initHitbox(int x, int y, int w, int h);


		void setDestRectDecalage(int x, int y);


		void incrDestRectDecalage(int x = 0, int y = 0);


		void setX(int _x); 
		

		void setY(int _y);  


		void setXY(int _x, int _y); 
		

		void setW(unsigned int val);
		

		void setH(unsigned int val);

		
		void incrX(double _x); 
		

		void incrY(double _y);


		void incrXY(double _x, double _y); 

		
		void setAngle(double radian);		
		

		void incrAngle(double radian);

		
		bool setImg(SDL_Renderer *ren, const char *path, bool destroy=false);
		

		void setImgRGB(SDL_Renderer *ren, const char *path, Color color_to_hide, bool destroy=false);
		

		void setTexture(SDL_Texture *tex, Game* g=nullptr, bool destroy=false);

		
		void setSrcX(unsigned int val); 
		

		void setSrcW(unsigned int val); 
		

		void setSrcY(unsigned int val); 
		

		void setSrcH(unsigned int val);


		void setSrc(SDL_Rect rec);


		void setDestW(unsigned int val);
		

		void setDestH(unsigned int val);


		void setDestWH(unsigned int _w, unsigned int _h); 


		void incrDestW(int val);


		void incrDestH(int val);


		void incrDestWH(int _w, int _h);


		void setState(const short val);


		void setState2(const short val);


		void setFlip(const SDL_RendererFlip flip);


		void setShow(const bool val);
		
		
		


	    // geters
		Hitbox& getHitbox(); 
		

		SDL_Rect& getHitboxRect(); 


		Hitbox getHitboxCpy(); 
		

		SDL_Rect getHitboxRectCpy(); 
		

		int getX() const; 


		double getX_f() const;


		int getW() const; 
		

		int getY() const; 


		double getY_f() const; 


		int getH() const;
		

		double getAngle() const;
		

		SDL_Rect* getSrc(); 
		

		int getSrcX() const; 


		int getSrcW() const; 
		

		int getSrcY() const; 


		int getSrcH() const;

		
		SDL_Rect* getDest();
		

		int getDestW();


		int getDestH();


		int getDestX();


		int getDestY();


		short getState() const;


		short getState2() const;


		SDL_RendererFlip getFlip() const;


		bool getShow() const;
		

		SDL_Texture *getImg();


		SDL_Texture* getTexture();


		bool isImgLoaded() const; 
		

		


		



		//ACTIONS 
		bool selfRenderCopy(Game* g, 
							const double angle_rad, 
							const SDL_RendererFlip flip = SDL_FLIP_NONE);
		

		bool selfRenderCopy(Game* g);


		bool selfRenderCopy(Game* g, const SDL_RendererFlip flip);
		

		void drawHitbox(Game* g, SDL_Color color={0, 0, 0, 255}, bool fill=false);

		
		bool isCollision(Body& body, 
						 int marge_dIgnorence_a = 0, 
						 int marge_dIgnorence_b = 0) const;


		bool willHit(Hitbox& hitbox, double self_move_x, double self_move_y, 
					 int marge_dIgnorence_a = 0, 
					 int marge_dIgnorence_b = 0,
					 int* dist_x_before_hit__out=nullptr,
                     int* dist_y_before_hit__out=nullptr) const;


		bool willHit(Body& body, double self_move_x, double self_move_y, 
					 int marge_dIgnorence_a = 0, 
					 int marge_dIgnorence_b = 0,
					 int* dist_x_before_hit__out=nullptr,
                     int* dist_y_before_hit__out=nullptr) const;



	

	protected : 

		Hitbox hitbox_;
		//SDL_Rect src, dest; 
		double angle_;
		SDL_RendererFlip m_flip=SDL_FLIP_NONE;
		short m_dest_decalage_x, m_dest_decalage_y;
		short etat_;
		short etat2_;
		double m_support_R_pos_x=0; //support pour travailler la position avec des réels. La position entière du Body sera déduite de ça.
		// float vecteurX;
		double m_support_R_pos_y=0;
		// float vecteurY; 
		int virtualPosX_;
		int virtualPosY_;
		bool m_show=true;
		//Body//
};



















//_________________________________________________________________________________
//_________________________________________________________________________________





class Entity : public Body
{
	public : 

		Entity() = delete;


		Entity(std::string _name, short nb_member = 1);


		~Entity(); 
				

		void setName(std::string _n);
		

		void inputUP(bool i);
		

		void inputDOWN(bool i);
		

		void inputLEFT(bool i);
		

		void inputRIGHT(bool i); 
		

		void input(std::string btn, bool i);


		void setSpeed(double val);


		// void setState(const short val);
		
		
		// void setState2(const short val);





		std::string getName() const;
		

		short getNbOfMembers() const;

	
		bool inputUP() const;
		

		bool inputDOWN() const;
		

		bool inputLEFT() const;
		

		bool inputRIGHT() const;
		

		bool input(std::string btn) const; 

		
		double getSpeed() const;
		

		// short getState() const;

		
		// short getState2() const;

		
		Body& member(unsigned int i);
	






		//actions 
		void moveAllBodies( Axis axe,  Signe signe, double _dt = 1.0); //avec vitesse du Entity
		

		void moveAllBodies( Axis axe, double speed_val, double _dt = 1.0 );   //avec vitesse précisée
		

		void selfRenderCopyAll(Game *g);
		
		
		void selfRenderCopyAll(Game *g, const double radian, SDL_RendererFlip flip = SDL_FLIP_NONE);
		

		void selfRenderCopyAll(Game *g, SDL_RendererFlip flip);

		
		bool isCollision( Entity &entity, int marge_dIgnorence_a = 0, int marge_dIgnorence_b = 0);
		

		bool isCollision( Body   &  body, int marge_dIgnorence_a = 0, int marge_dIgnorence_b = 0);




	private : 
		std::string name_;
		std::vector<Body> member_;
		Input input_;
		short nb_mb;
		// short etat_;
		// short etat2_;
		double speed_;
		//Entity//

}; 


//_________________________________________________________________________________
//_________________________________________________________________________________








//------------------------les collisions---------------------------// 


bool isCollisionBetweenTwoHitboxes(const Hitbox& a, const Hitbox& b, 
			    int marge_dIgnorence = 0, int marge_dIgnorence_b = 0);

Hitbox hitboxOf(int x, int y, int w, int h);

Hitbox hitboxOf(Body &body);

bool willHit(Body& body1, double body1_move_x, double body1_move_y, 
             Body& body2, double body2_move_x, double body2_move_y);

// bool willHit(Entity& entity1, double body1_move_x, double body1_move_y, 
//              Entity& entity2, double body2_move_x, double body2_move_y);
//-> à surcharger


//-----------------------------------------------------------------// 


unsigned int distOnX(const Hitbox& hitbox1, const Hitbox& hitbox2);
unsigned int distOnY(const Hitbox& hitbox1, const Hitbox& hitbox2);
unsigned int distOnX(Body& body1, Body& body2);
unsigned int distOnY(Body& body1, Body& body2);
double euclideanDist(const Hitbox& hitbox1, const Hitbox& hitbox2, en::Corner corner);
double euclideanDist(Body& body1, Body& body2, en::Corner corner);
bool touchOnX(const Hitbox& hitbox1, const Hitbox& hitbox2);
bool touchOnY(const Hitbox& hitbox1, const Hitbox& hitbox2);
bool touchOnX(Body& body1, Body& body2);
bool touchOnY(Body& body1, Body& body2);
bool fstTouchSndSUp(const Hitbox& fst, const Hitbox& snd);
bool fstTouchSndSDown(const Hitbox& fst, const Hitbox& snd);
bool fstTouchSndSLeft(const Hitbox& fst, const Hitbox& snd);
bool fstTouchSndSRight(const Hitbox& fst, const Hitbox& snd);
bool fstTouchSndSUp(Body& fst, Body& snd);
bool fstTouchSndSDown(Body& fst, Body& snd);
bool fstTouchSndSLeft(Body& fst, Body& snd);
bool fstTouchSndSRight(Body& fst, Body& snd);








};/*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*/



/*
- rajouter un systeme de déplacement des corps avec des positions relatif les uns aux autres 
(e.g : déplacler de -5x;-16y de un tel)

- rajouter une fonction pour changer l'angle de tous les membres par une même valeur

- rajouter une fonction qui met tous les membre à une même position 


*/

#endif