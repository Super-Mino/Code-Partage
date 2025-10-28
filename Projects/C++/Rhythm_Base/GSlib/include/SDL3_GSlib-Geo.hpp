#ifndef _SDL3_GSlib_GEO_HPP_
#define _SDL3_GSlib_GEO_HPP_

//Version modifiée le 19/10/25 (à moins d'oubli de modifier cette ligne ou autre).


#include "SDL3_GSlib.hpp"

namespace gs
{

struct Shape_P //Shape parameters.
{
	SDL_FRect area;

	enum Shape {E, R, T, S}; //Ellipse, rect, triangle, segment.
	
	Shape type;
	Vec2f pos={0.1,0.1}, pos2={0.1,0.2}, pos3={0.2,0.15}; //Pourcentages des tailles de 'area', afin se postionner relativement à son coin supérieur gauche.
	Vec2f size={0.1,0.1}; //Pourcentages des tailles de 'area'.
	
	bool fill=false;
	bool clockwise=true; //Sens de rotation. 

	float rot_ang=0;
	Vec2f rot_pt; //Pour les rectangles c'est relatif au rectangle et à son coin supérieur gauche : {0,0} = coin supérieur gauche du rectangle, {1,1} = coin inférieur droit, {0.5, 0.5} = (plus ou moins) le milieu. 
					//Pour les triangles {0,0} = centre du triangle, sinon c'est relatif à 'area' (un peu comme pour 'pos', 'pos2', 'pos3').
					//Pour les segements c'est aussi comme pour 'pos', 'pos2', 'pos3'.

	//Pour les ellipses.
	float total_angle=2*_pi_;
	float begin_angle=0;

	float thickness=0; //Si 0 : pas prise en compte (drawEllipse()), sinon prise en compte (drawEllipseThickness()), avec 'thickness' relative à 'area.w';

	float round_percent=0;

	SDL_FColor color={255.f,255.f,255.f,255.f};
	SDL_BlendMode blend_mode=SDL_BLENDMODE_BLEND;
	bool set_blend_mode=false;

};


struct Rotation_P 
{
	float rot_rad=0.f; 
	gs::Vec2f anchor={0,0}; 
	bool clockwise=true;
};


struct Triangle
{
	Vec2f a, b, c;
	
	bool draw(SDL_Renderer* ren, const SDL_FColor* col=nullptr, const bool& fill=false, const SDL_BlendMode* b_mode=nullptr);
	
	void move(const gs::Vec2f& speed);
	
	void rotate(const double& rad, const Vec2f* rot_center=nullptr, const bool& clockwise=true);
	
	gs::Vec2f getCenter();
	
};



struct Circle
{
	Vec2d center;
	double radius;

	bool draw(SDL_Renderer* ren, const SDL_FColor& col={100.f,0.f,0.f,255.f}, const bool& fill=false, float total_angle=2*_pi_, float begin_angle=0);
	
	bool drawThickness(SDL_Renderer* ren, int thickness, const SDL_FColor& col, const bool& fill=false, float total_angle=2*_pi_, float begin_angle=0);

};




struct Line
{

	double a, b, c;
	Line()=default;
	
	Line(double a, double b, double c);
	Line(const Vec2d& point, const Vec2d& dir_vector); //Création d'une droite à partir d'un point et d'un vecteur. 
	Line(double leading_coef, double y_intercept); //Avec coefficient directeur et ordonnée à l'origine.
	
	void initWith2pts(const Vec2d& pt1, const Vec2d& pt2); //Droite passant par deux points.

	void draw(SDL_Renderer* ren, const SDL_FColor& color={200.f,100.f,0.f,255.f}, int win_w=2000, int win_h=2000);

	bool intersect(const Line& ln) const;
	
	Vec2d intersectPoint(const Line& ln);
	
	Vec2i intersectPoint_i(const Line& ln);

	bool intersect(const Circle& cir, Vec2d* out__pt1=nullptr, Vec2d* out__pt2=nullptr) const;

	bool intersect_i(const Circle& cir, Vec2i* out__pt1=nullptr, Vec2i* out__pt2=nullptr) const;


};





struct Line_Segment
{
	Vec2d a, z; //Les deux bornes du segment.
	Line_Segment()=default;
	Line_Segment(const Vec2d& a, const Vec2d& z);
	Line_Segment(const Vec2d& o, const Vec2d& dir_vector, const double& len); //D'après un point d'origine, un vecteur directeur et un longueur (en pixel). 

	void draw(SDL_Renderer* ren, const SDL_FColor& color={0,200,200,255}) const;
	
	
	bool intersect(const Line_Segment& ln_s) const; //Intersection entre deux segments.
	
	Vec2d intersectPoint(const Line_Segment& ln_s) const;
	
	Vec2i intersectPoint_i(const Line_Segment& ln_s) const;
	


	bool intersect(const Line& ln) const; //Intersection entre un segment et une droite.
	
	Vec2d intersectPoint(const Line& ln) const;
	
	Vec2i intersectPoint_i(const Line& ln) const;

	Vec2b intersect(const Circle& cir, Vec2d* out__pt1=nullptr, Vec2d* out__pt2=nullptr) const; //Intersection entre un segment et un cercle. Renvoie deux booléens car il peut ou ne pas y avoir l'intersection n°1 et de même pour l'intersection n°2.
	
	Vec2b intersect_i(const Circle& cir, Vec2i* out__pt1=nullptr, Vec2i* out__pt2=nullptr) const; // (avec des int) Intersection entre un segment et un cercle. Renvoie deux booléens car il peut ou ne pas y avoir l'intersection n°1 et de même pour l'intersection n°2.



	Line getLine() const;

	double getLen() const;
	
	int getLen_i() const;
	
};





bool drawRect(SDL_Renderer* ren, const SDL_FRect* rect, const SDL_FColor* col, const bool& fill, SDL_BlendMode* b_mode, const Rotation_P& rotation);

bool drawRoundedRect(SDL_Renderer* ren, const SDL_FRect& rect, float percent, const SDL_FColor& col, bool fill=false, SDL_BlendMode* b_mode=nullptr);

bool drawEllipse(SDL_Renderer* ren, const Vec2i& center, const Vec2i& radii, const SDL_FColor& col, const bool& fill=false, float total_angle=2*_pi_, float begin_angle=0);

bool drawEllipseThickness(SDL_Renderer* ren, const Vec2i& center, Vec2i radii, int thickness, const SDL_FColor& col, const bool& fill=false, float total_angle=2*_pi_, float begin_angle=0);

bool drawCircle(SDL_Renderer* ren, const Vec2i& center, int radius, const SDL_FColor& col, const bool& fill=false, float total_angle=2*_pi_, float begin_angle=0);

bool drawCircleThickness(SDL_Renderer* ren, const Vec2i& center, int radius, int thickness, const SDL_FColor& col, const bool& fill=false, float total_angle=2*_pi_, float begin_angle=0);


bool drawRect(SDL_Renderer* ren, const SDL_FRect& rect, const SDL_FColor* col, const bool& fill, SDL_BlendMode* b_mode, const Rotation_P& rotation);
bool drawRect(SDL_Renderer* ren, const SDL_FRect* rect, const SDL_FColor& col, const bool& fill, SDL_BlendMode* b_mode, const Rotation_P& rotation);
bool drawRect(SDL_Renderer* ren, const SDL_FRect& rect, const SDL_FColor& col, const bool& fill, SDL_BlendMode* b_mode, const Rotation_P& rotation);


bool drawShape(SDL_Renderer* ren, Shape_P s);



}

std::ostream& operator<<(std::ostream& os, const gs::Line& ln);



#endif
