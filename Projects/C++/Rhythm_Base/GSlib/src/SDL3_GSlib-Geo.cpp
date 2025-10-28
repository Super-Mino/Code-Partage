#include "SDL3_GSlib-Geo.hpp"

//Version modifiée le 19/10/25 (à moins d'oubli de modifier cette ligne, ou autre).

std::ostream& operator<<(std::ostream& os, const gs::Line& ln)
{
	os << "[Ln -> a " << ln.a << "| b " << ln.b << "| c " << ln.c << "]";
	return os;
}



namespace gs
{


bool Triangle::draw(SDL_Renderer* ren, const SDL_FColor* col, const bool& fill, const SDL_BlendMode* b_mode)
{
	if(!ren) 
	{
		gs::lastError("||Warning|| in 'Triangle::draw()', invalid renderer. ");
		return false;
	}

	SDL_BlendMode previous_blend_mode;
	if(b_mode)
	{
		SDL_GetRenderDrawBlendMode(ren, &previous_blend_mode);
		SDL_SetRenderDrawBlendMode(ren, *b_mode);
	}

	SDL_Color previous_c;
	SDL_FColor color_;
	if(col) //Application de la couleur demandée. 
	{
		SDL_GetRenderDrawColor(ren,&previous_c.r,&previous_c.g,&previous_c.b,&previous_c.a);
		SDL_SetRenderDrawColor(ren, col->r, col->g, col->b, col->a);
		color_.r=col->r; color_.g=col->g; color_.b=col->b; color_.a=col->a;
	}
	else
	{
		SDL_Color tmp_col;
		SDL_GetRenderDrawColor(ren, &tmp_col.r, &tmp_col.g, &tmp_col.b, &tmp_col.a);
		color_.r=tmp_col.r; color_.g=tmp_col.g; color_.b=tmp_col.b; color_.a=tmp_col.a;
	}
	
	bool success = true;
	bool res, res1, res2, res3;

	if(fill)
	{
		SDL_Vertex common_fields;
		common_fields.color = color_;
		common_fields.tex_coord.x = 1;
		common_fields.tex_coord.y = 1;

		std::vector<SDL_Vertex> vertices={common_fields, common_fields, common_fields};
		vertices.at(0).position.x = a.x; vertices.at(0).position.y = a.y; 
		vertices.at(1).position.x = b.x; vertices.at(1).position.y = b.y;
		vertices.at(2).position.x = c.x; vertices.at(2).position.y = c.y;

		res = SDL_RenderGeometry(ren, nullptr, vertices.data(), int(vertices.size()), nullptr, 0);
	}
	else
	{	
		res1 = SDL_RenderLine(ren, (int)a.x, (int)a.y, (int)b.x, (int)b.y);
		res2 = SDL_RenderLine(ren, (int)b.x, (int)b.y, (int)c.x, (int)c.y);
		res3 = SDL_RenderLine(ren, (int)c.x, (int)c.y, (int)a.x, (int)a.y);
	}
		
	if(!res or !res1 or !res2 or !res3)
	{
		using std::operator ""s;
		gs::lastError("||Warning|| in 'Triangle::draw()' : "s + SDL_GetError());
		success = false;
	}

	if(col) //Restauration de la couleur de dessin précédente. 
		SDL_SetRenderDrawColor(ren,previous_c.r,previous_c.g,previous_c.b,previous_c.a);
	
	if(b_mode) //Restauration du b_mode précédente. 
		SDL_SetRenderDrawBlendMode(ren, previous_blend_mode);

	return success;

}

void Triangle::move(const gs::Vec2f& speed)
{
	a += speed;
	b += speed;
	c += speed;
}

void Triangle::rotate(const double& rad, const Vec2f* rot_center, const bool& clockwise)
{
	std::vector<gs::Vec2f*> pts={&a, &b, &c};
	
	Vec2f center = (rot_center ? *rot_center : getCenter());

	for(Vec2f* pt : pts)
	{
		Vec2f new_pos = rotatePoint(center.x, center.y, pt->x, pt->y, rad, clockwise).to<float>();
		*pt = new_pos;
	}
}

Vec2f Triangle::getCenter()
{
	float center_x = (a.x + b.x + c.x) / 3.0;
	float center_y = (a.y + b.y + c.y) / 3.0;

	return {center_x, center_y};
}
















Line::Line(double a, double b, double c)
: a(a), b(b), c(c) 
{}

Line::Line(const gs::Vec2d& point, const gs::Vec2d& dir_vector) //Création d'une droite à partir d'un point et d'un vecteur. 
: a(dir_vector.y), b(-dir_vector.x), c(-(dir_vector.y*point.x + (-dir_vector.x)*point.y))
{}

Line::Line(double leading_coef, double y_intercept)
: a(leading_coef), b(-1), c(y_intercept)
{}

void Line::initWith2pts(const Vec2d& pt1, const Vec2d& pt2) //Droite passant par deux points.
{
	gs::Vec2d&& dir_vector = pt2-pt1;

	a = dir_vector.y;
	b = -dir_vector.x;
	c = -(dir_vector.y*pt1.x + (-dir_vector.x)*pt1.y);
}



void Line::draw(SDL_Renderer* ren, const SDL_FColor& color, int win_w, int win_h)
{
	// if(b == 0) return;
	// if(a == 0) return;

	if(win_w<0) win_w = 2000;
	if(win_h<0) win_h = 2000;

	double y1; 
	double x1;

	double y2;
	double x2;

	if(a == 0) //Ligne horizontal. 
	{
		y1=c/(-b);
		y2=y1;

		x1=0;
		x2=win_w;
	}
	else
	{
		y1 = 0;
		x1 = ((-b)*y1-c)/a;

		y2 = win_h;
		x2 = ((-b)*y2-c)/a;
	}

	
	//Pour éviter les valeurs trop grandes, on inverse les axes de référence.
	if(std::abs(x1) > 2147483640)
	{
		x1 = (x1 < 0 ? 0 : win_w);
		y1 = ((-(a*x1))-c)/b;
	}

	if(std::abs(x2) > 2147483640)
	{
		x2 = (x2 < 0 ? 0 : win_w);
		y2 = ((-(a*x2))-c)/b;
	}

	gs::drawLine(ren, gs::Vec2i(x1,y1), gs::Vec2i(x2,y2), color);
}

bool Line::intersect(const Line& ln) const
{
	//On vérifie si le vecteur directeur de l'un et le vecteur normal de l'autre sont orthogonaux.
	//Si oui, alors les droites sont parallèles : il n'y a pas de point d'intersection. 

	gs::Vec2d u={-b, a};
	gs::Vec2d n={ln.a, ln.b}; 

	if( (u.x*n.x + u.y*n.y) == 0 )
		return false; 
	
	return true; 
}

Vec2d Line::intersectPoint(const Line& ln) 
{
	//Retourne le point d'intersection s'il y'en a. 

	Vec2d pt;

	if(b == 0)
		b = -0.000000001; 
	

	double x_denominator = ln.b*a - b*ln.a;
	
	if(x_denominator == 0) 
		x_denominator = -0.000000001;
	

	pt.x = (-(c*ln.b) + b*ln.c) / x_denominator;
	pt.y = (-a* ((-(c*ln.b) + b*ln.c) / x_denominator) -c)/b; 

	return pt;
}

Vec2i Line::intersectPoint_i(const Line& ln) 
{
	//Retourne le point d'intersection s'il y'en a. (version int).
	Vec2d pt = intersectPoint(ln);

	return gs::Vec2i(int(pt.x), int(pt.y));
}

bool Line::intersect(const Circle& cir, Vec2d* out__pt1, Vec2d* out__pt2) const
{
	return lineAndCircleIntersect(a, b, c, cir.center, cir.radius, out__pt1, out__pt2);
}

bool Line::intersect_i(const Circle& cir, Vec2i* out__pt1, Vec2i* out__pt2) const
{
	return lineAndCircleIntersect_i(a, b, c, cir.center, cir.radius, out__pt1, out__pt2);
}
















Line_Segment::Line_Segment(const gs::Vec2d& a, const gs::Vec2d& z)
: a(a), z(z)
{}

Line_Segment::Line_Segment(const gs::Vec2d& o, const gs::Vec2d& dir_vector, const double& len) //D'après un point d'origine, un vecteur directeur et un longueur (en pixel). 
: a(o)
{
	//Normalisation du vecteur.
	gs::Vec2d dir_vec = normalizedVecOf(dir_vector);

	z.x = a.x + dir_vec.x*len;
	z.y = a.y + dir_vec.y*len;
}


void Line_Segment::draw(SDL_Renderer* ren, const SDL_FColor& color) const
{
	gs::drawLine(ren, gs::Vec2i(int(a.x), int(a.y)), gs::Vec2i(int(z.x), int(z.y)), color);
}


bool Line_Segment::intersect(const Line_Segment& ln_s) const //Intersection entre deux segments.
{
	
	Line&& self_line = getLine();
	Line&& ln_s_line = ln_s.getLine();

	if(not self_line.intersect(ln_s_line))
		return false;

	gs::Vec2d pt = self_line.intersectPoint(ln_s_line);

	// double self_len = gs::pointsDist(a, z);

	// if((gs::pointsDist(a, pt) <= self_len) and (gs::pointsDist(z, pt) <= self_len))
		// return true;

	//Si l'intersection se fait dans les intervalles communs aux deux segments. 
	if( (SDL_min(a.x, z.x) <= pt.x) and (pt.x <= SDL_max(a.x, z.x)) )
	if( (SDL_min(a.y, z.y) <= pt.y) and (pt.y <= SDL_max(a.y, z.y)) )
	if( (SDL_min(ln_s.a.x, ln_s.z.x) <= pt.x) and (pt.x <= SDL_max(ln_s.a.x, ln_s.z.x)) )
	if( (SDL_min(ln_s.a.y, ln_s.z.y) <= pt.y) and (pt.y <= SDL_max(ln_s.a.y, ln_s.z.y)) )
		return true;


	return false;
}


gs::Vec2d Line_Segment::intersectPoint(const Line_Segment& ln_s) const
{
	return getLine().intersectPoint(ln_s.getLine());
}
gs::Vec2i Line_Segment::intersectPoint_i(const Line_Segment& ln_s) const
{
	gs::Vec2d pt = intersectPoint(std::move(ln_s));
	return gs::Vec2i(int(pt.x), int(pt.y));
}


bool Line_Segment::intersect(const Line& ln) const //Intersection entre un segment et une droite.
{
	Line&& self_line = getLine();
	
	if(not self_line.intersect(ln))
		return false;
	

	gs::Vec2d pt = self_line.intersectPoint(ln);

	double self_len = gs::pointsDist(a, z);

	if((gs::pointsDist(a, pt) <= self_len) and (gs::pointsDist(z, pt) <= self_len))
		return true;


	return false;
}

gs::Vec2d Line_Segment::intersectPoint(const Line& ln) const
{
	return getLine().intersectPoint(ln);
}
gs::Vec2i Line_Segment::intersectPoint_i(const Line& ln) const
{
	gs::Vec2d pt = intersectPoint(std::move(ln));
	return gs::Vec2i(int(pt.x), int(pt.y));
}

Vec2b Line_Segment::intersect(const Circle& cir, Vec2d* out__pt1, Vec2d* out__pt2) const
{
	Vec2b res = {false, false};
	Vec2d pt1, pt2;

	if(getLine().intersect(cir, &pt1, &pt2)) //Si au moins le cercle et la droite correspondate du segment se croisent.
	{
		//On vérifie pour chaque point s'il est sur le segment.
		//Autrement dit si l'intersection se fait dans les intervalles du segments. 
		if( (SDL_min(a.x, z.x) <= pt1.x) and (pt1.x <= SDL_max(a.x, z.x)) )
		if( (SDL_min(a.y, z.y) <= pt1.y) and (pt1.y <= SDL_max(a.y, z.y)) )
		{
			res.x = true;
			if(out__pt1)
				(*out__pt1) = pt1;
		}

		if( (SDL_min(a.x, z.x) <= pt2.x) and (pt2.x <= SDL_max(a.x, z.x)) )
		if( (SDL_min(a.y, z.y) <= pt2.y) and (pt2.y <= SDL_max(a.y, z.y)) )
		{
			res.y = true;
			if(out__pt2)
				(*out__pt2) = pt2;
		}
	}

	return res;
}




Vec2b Line_Segment::intersect_i(const Circle& cir, Vec2i* out__pt1, Vec2i* out__pt2) const
{
	Vec2d pt1, pt2;
	Vec2b res = this->intersect(cir, &pt1, &pt2);

	if(res.x)
		(*out__pt1) = pt1.to<int>();

	if(res.y)
		(*out__pt2) = pt2.to<int>();

	return res;
}




Line Line_Segment::getLine() const
{
	gs::Vec2d u_vec = {z.x-a.x, z.y-a.y};

	return Line(z, u_vec);
}

double Line_Segment::getLen() const
{
	return gs::pointsDist(a, z);
}
int Line_Segment::getLen_i() const
{
	return int(gs::pointsDist(a, z));
}















bool Circle::draw(SDL_Renderer* ren, const SDL_FColor& col, const bool& fill, float total_angle, float begin_angle)
{
	return drawCircle(ren, center.to<int>(), int(radius), col, fill, total_angle, begin_angle);
}

bool Circle::drawThickness(SDL_Renderer* ren, int thickness, const SDL_FColor& col, const bool& fill, float total_angle, float begin_angle)
{
	return drawCircleThickness(ren, center.to<int>(), int(radius), thickness, col, fill, total_angle, begin_angle);
}
















bool drawRect(SDL_Renderer* ren, const SDL_FRect* rect, const SDL_FColor* col, const bool& fill, /*short thickness,*/ SDL_BlendMode* b_mode, const Rotation_P& rotation)
{
	if(not ren) 
	{
		gs::lastError("In 'drawRect()', invalid renderer. ");
		return false;
	}

	const float& rot_rad = rotation.rot_rad;
	const gs::Vec2f& anchor = rotation.anchor;
	const bool& clockwise = rotation.clockwise;


	SDL_FRect rect_cpy={0.f, 0.f, 0.f, 0.f};
	
	if(rect) //S'il y'a un rectangle de donné.
	{
		rect_cpy = *rect;
		//rect_cpy.x = float(rect->x);
		//rect_cpy.y = float(rect->y);
		//rect_cpy.w = float(rect->w);
		//rect_cpy.h = float(rect->h);
	}
	else //Sinon on prend le rectangle de l'affichage.
	{
		int w, h;
		SDL_GetCurrentRenderOutputSize(ren, &w, &h); 
		rect_cpy.w = float(w);
		rect_cpy.h = float(h);
	}


	//Les quatres points du rectangle
	//
	// nw       ne
	//
	// sw       se
	gs::Vec2f nw, ne, se, sw;
	nw.x = rect_cpy.x;
	nw.y = rect_cpy.y;
	ne.x = nw.x + rect_cpy.w-1;
	ne.y = nw.y;
	se.x = ne.x;
	se.y = ne.y + rect_cpy.h-1;
	sw.x = nw.x;
	sw.y = se.y;

	

	//Centre de rotation.
	gs::Vec2f rot_center;
	rot_center.x = rect_cpy.x + (rect_cpy.w*anchor.x);
	rot_center.y = rect_cpy.y + (rect_cpy.h*anchor.y);


	//Calculs des coordonnées des différents points par rapport au centre de rotation après rotation. 
	if(rot_rad != 0)
	{
		nw = rotatePoint(rot_center.x, rot_center.y, nw.x, nw.y, rot_rad, clockwise).to<float>();
		ne = rotatePoint(rot_center.x, rot_center.y, ne.x, ne.y, rot_rad, clockwise).to<float>();
		se = rotatePoint(rot_center.x, rot_center.y, se.x, se.y, rot_rad, clockwise).to<float>();
		sw = rotatePoint(rot_center.x, rot_center.y, sw.x, sw.y, rot_rad, clockwise).to<float>();
	}

	//Préparation au dessin
	SDL_BlendMode previous_blend_mode;
	if(b_mode)
	{
		SDL_GetRenderDrawBlendMode(ren, &previous_blend_mode);
		SDL_SetRenderDrawBlendMode(ren, *b_mode);
	}

	SDL_Color previous_c;
	SDL_FColor color_;
	if(col)
	{
		SDL_GetRenderDrawColor(ren, &previous_c.r, &previous_c.g, &previous_c.b, &previous_c.a);
		SDL_SetRenderDrawColor(ren, col->r, col->g, col->b, col->a); 
		color_.r=col->r; color_.g=col->g; color_.b=col->b; color_.a=col->a;
	}
	else
	{
		SDL_Color tmp_col;
		SDL_GetRenderDrawColor(ren, &tmp_col.r, &tmp_col.g, &tmp_col.b, &tmp_col.a);
		color_.r=tmp_col.r; color_.g=tmp_col.g; color_.b=tmp_col.b; color_.a=tmp_col.a;
	}
	
	
	
	//Dessin
	if(fill)
	{
		SDL_Vertex common_fields;
		common_fields.color = color_;
		common_fields.tex_coord.x = 1;
		common_fields.tex_coord.y = 1;

		std::vector<SDL_Vertex> vertices{common_fields, common_fields, common_fields};
		vertices.at(0).position.x = nw.x;
		vertices.at(0).position.y = nw.y;
		vertices.at(1).position.x = ne.x;
		vertices.at(1).position.y = ne.y;
		vertices.at(2).position.x = se.x;
		vertices.at(2).position.y = se.y;

		bool res1 = SDL_RenderGeometry(ren, nullptr, vertices.data(), int(vertices.size()), nullptr, 0);
		vertices.at(1).position.x = sw.x;
		vertices.at(1).position.y = sw.y;
		bool res2 = SDL_RenderGeometry(ren, nullptr, vertices.data(), int(vertices.size()), nullptr, 0);
	}
	else
	{
		std::vector<SDL_FPoint> points{nw.toSDL_FPoint(), ne.toSDL_FPoint(), 
										se.toSDL_FPoint(), sw.toSDL_FPoint(), 
										nw.toSDL_FPoint()};

		SDL_RenderLines(ren, points.data(), 5);
	}


	//Rétablissement des précédents paramètres. 
	if(col)
		SDL_SetRenderDrawColor(ren, previous_c.r, previous_c.g, previous_c.b, previous_c.a);
	
	if(b_mode)
		SDL_SetRenderDrawBlendMode(ren, previous_blend_mode);

	
	return true;
}



bool drawRoundedRect(SDL_Renderer* ren, const SDL_FRect& rect, float percent, const SDL_FColor& col, bool fill, SDL_BlendMode* b_mode)
{
	if(!ren)
	{
		gs::lastError("||Error|| in 'drawRoundedRect()', invalid renderer. ");
		return false;
	} 
		 
	if(SDL_RectEmptyFloat(&rect)) return true; 

	if(percent < 0.f or percent > 0.5f) percent = 0.5f;

	float min_side_size = SDL_min(rect.w, rect.h);
	float margin = min_side_size * percent;

	SDL_FRect main = {rect.x+margin, rect.y, rect.w-margin*2, rect.h};
	SDL_FRect left_side = {rect.x, rect.y+margin, margin, rect.h-margin*2};
	SDL_FRect right_side = {main.x+main.w, rect.y+margin, margin, rect.h-margin*2};
 

	//Préparation au dessin
	SDL_BlendMode previous_blend_mode;
	if(b_mode)
	{
		SDL_GetRenderDrawBlendMode(ren, &previous_blend_mode);
		SDL_SetRenderDrawBlendMode(ren, *b_mode);
	}

	bool success = true;

	if(fill)
	{
		success = success & drawRect(ren, &main, &col, fill);
		success = success & drawRect(ren, &left_side, &col, fill);
		success = success & drawRect(ren, &right_side, &col, fill);
	}
	else
	{
		Vec2i line_1_pt1 = Vec2f(main.x, main.y).to<int>();
		Vec2i line_1_pt2 = Vec2f(main.x+main.w-1, main.y).to<int>();
		Vec2i line_2_pt1 = Vec2f(right_side.x+margin-1, right_side.y).to<int>();
		Vec2i line_2_pt2 = Vec2f(right_side.x+margin-1, right_side.y+right_side.h-1).to<int>();
		Vec2i line_3_pt1 = Vec2f(main.x, main.y+main.h-1).to<int>();
		Vec2i line_3_pt2 = Vec2f(main.x+main.w-1, main.y+main.h-1).to<int>();
		Vec2i line_4_pt1 = Vec2f(left_side.x, left_side.y).to<int>();
		Vec2i line_4_pt2 = Vec2f(left_side.x, left_side.y+left_side.h-1).to<int>();

		success = success & drawLine(ren, line_1_pt1, line_1_pt2, col);
		success = success & drawLine(ren, line_2_pt1, line_2_pt2, col);
		success = success & drawLine(ren, line_3_pt1, line_3_pt2, col);
		success = success & drawLine(ren, line_4_pt1, line_4_pt2, col);
	}
	
	Vec2i margin_margin = Vec2f(margin, margin).to<int>();
	success = success & drawEllipse(ren, Vec2f(rect.x+margin, rect.y+margin).to<int>(), margin_margin, col, fill, gs::rad(90), gs::rad(180));
	success = success & drawEllipse(ren, Vec2f(right_side.x-(int(!fill)), right_side.y).to<int>(), margin_margin, col, fill, gs::rad(90), gs::rad(270));
	success = success & drawEllipse(ren, Vec2f(right_side.x-(int(!fill)), right_side.y+right_side.h-(int(!fill))).to<int>(), margin_margin, col, fill, gs::rad(90), gs::rad(0));
	success = success & drawEllipse(ren, Vec2f(rect.x+margin, left_side.y+left_side.h-(int(!fill))).to<int>(), margin_margin, col, fill, gs::rad(90), gs::rad(90));
	

	if(not success)
		gs::lastError(gs::str("||Warning|| in 'drawRoundedRect()', error occured : ")+gs::lastError());
	

	if(b_mode)
		SDL_SetRenderDrawBlendMode(ren, previous_blend_mode);

	return success;
}



bool drawEllipse(SDL_Renderer* ren, const Vec2i& center, const Vec2i& radii, const SDL_FColor& col, const bool& fill, float total_angle, float begin_angle)
{
	//Dessine une ellipse par décompositon en triangles (ou en segements si ce n'est pas plein). 

	if(!ren) 
	{
		gs::lastError("||Error|| in 'drawEllipse()', invalid renderer. ");
		return false;
	}

	if(total_angle <= 0)
	{
		// gs::lastError("||Warning|| in 'drawEllipse()', 'total_angle' must be '>= 0' ");
		return true; // return false;
	} 
	if(total_angle > 2*_pi_)
		total_angle = 2*_pi_;

	static const char qualities_nb=8;
	static const short qualities[qualities_nb] = {12, 24, 30, 60, 70, 80, 100, 120}; // -> en combien de triangle on va découper le cerlce. 
	static const short limits[qualities_nb-1] = {10, 30, 100, 220, 300, 400, 500}; 

	float nb_of_triangles = float(qualities[qualities_nb-1]);
	int max_radius = SDL_max(radii.x, radii.y); 
	
	for(int i=0; i < qualities_nb-1; ++i)
	{
		if(max_radius < limits[i])
		{
			nb_of_triangles = float(qualities[i]);
			break;
		}
	}



	float percent_angle = total_angle / (2.0*M_PI); 
	nb_of_triangles = float(int(nb_of_triangles * percent_angle));

	SDL_FPoint circle_center={float(center.x), float(center.y)};
	float delta_angle = total_angle / nb_of_triangles; //2*M_PI / nb_of_triangles; 
	
	float curr_angle = begin_angle;
	std::vector<SDL_Vertex> vertices; 

	SDL_Vertex common_fields;
		common_fields.color = col;
		common_fields.tex_coord.x = 1;
		common_fields.tex_coord.y = 1;
		
		//L'arg 'col' est attendu au format [0; 255], cependant 'SDL_Vertex.color' est attendu au format [0.0; 1.0], d'où la conversion.
		common_fields.color.r/=255.f; common_fields.color.g/=255.f; common_fields.color.b/=255.f; common_fields.color.a/=255.f;


	SDL_FPoint last_point; // Va être init avec le 1er point.
	bool break_next = false;

		last_point.x = std::cos(curr_angle) * float(radii.x) + circle_center.x;
		last_point.y = std::sin(curr_angle) * float(radii.y) + circle_center.y;

		//On tourne l'angle.
		if((curr_angle + delta_angle) > total_angle +begin_angle)
		{
			curr_angle = total_angle +begin_angle;
			break_next = true;
		}
		else
			curr_angle += delta_angle;

	
	
	do
	{
		SDL_FPoint curr_pt;

		curr_pt.x = std::cos(curr_angle) * float(radii.x) +circle_center.x;
		curr_pt.y = std::sin(curr_angle) * float(radii.y) +circle_center.y;

		if(fill)
		{
			vertices.push_back(common_fields);
				vertices.back().position = circle_center; // Point n°1.
		}
		
		vertices.push_back(common_fields);
			vertices.back().position = last_point; // Point n°2.

		vertices.push_back(common_fields);
			vertices.back().position = curr_pt; // Point n°3.


		last_point = curr_pt;

	
		if(break_next) break;

		// On tourne l'angle.
		if((curr_angle + delta_angle) > total_angle +begin_angle)
		{
			curr_angle = total_angle +begin_angle;
			break_next = true;
		}
		else
			curr_angle += delta_angle;
	}
	while(curr_angle <= total_angle +begin_angle);


	bool res;

	if(fill)
		res = SDL_RenderGeometry(ren, nullptr, vertices.data(), int(vertices.size()), nullptr, 0);
	else
	{
		std::vector<SDL_FPoint> points;

		for(SDL_Vertex& ver : vertices)
			points.push_back(ver.position);
		

		SDL_Color previous_c;
		SDL_GetRenderDrawColor(ren, &previous_c.r, &previous_c.g, &previous_c.b, &previous_c.a);
		SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.a); 
		
		res = SDL_RenderLines(ren, points.data(), points.size());

		SDL_SetRenderDrawColor(ren, previous_c.r, previous_c.g, previous_c.b, previous_c.a);
	}

	if(!res)
	{
		gs::lastError(gs::str("||Error|| in 'drawEllipse()' : ") + SDL_GetError());
		return false;
	}


// {
// 	std::cout << radii << "_________" << nb_of_triangles << "                 (" << int(float(vertices.size())*(2/3.f)) << ")\n";
// }

	return true;
}



bool drawEllipseThickness(SDL_Renderer* ren, const Vec2i& center, Vec2i radii, int thickness, const SDL_FColor& col, const bool& fill, float total_angle, float begin_angle)
{
	if(!ren) 
	{
		gs::lastError("||Error|| in 'drawEllipseThickness()', invalid renderer. ");
		return false;
	}

	if(total_angle < 0)
	{
		gs::lastError("||Warning|| in 'drawEllipseThickness()', 'total_angle' must be '>= 0'. ");
		return false;
	}
	else
	if(total_angle > 2*_pi_)
		total_angle = 2*_pi_;


float quality = 50;

	if(thickness == 0) 
	{
		gs::lastError("||Warning|| in 'drawEllipseThickness()', 'thickness' must be '!= 0'. ");
		return false;
	}
	else 
	if(thickness == 1 or thickness == -1)
	{
		radii += (thickness == -1 ? 1 : 0);
		bool success = drawEllipse(ren, center, radii, col, false, total_angle, begin_angle);
		if(not success)
			gs::lastError(gs::str("from 'drawEllipseThickness()' : ")+gs::lastError());

		return success;
	}
	else
	if(thickness >= SDL_min(radii.x, radii.y) and fill) //Si l'épaisseur dépasse le rayon minimum, l'épaisseur ne compte plus. 
	{
		bool success = drawEllipse(ren, center, radii, col, true, total_angle, begin_angle);
		if(not success)
			gs::lastError(gs::str("from 'drawEllipseThickness()' : ")+gs::lastError());

		return success;
	}

	if(thickness < 0) //Si l'épaisseur donnée est négative, on considère une épaisseur vers l'extérieur.
	{
		thickness = std::abs(thickness); 
		radii.x += thickness;
		radii.y += thickness;
	}

	
	if(thickness >= SDL_min(radii.x, radii.y)) //Si ce teste passe, alors c'est que fill==false.
		thickness = SDL_min(radii.x, radii.y);

	//Les rayons internes.
	float min_radius_x = radii.x - thickness;
	float min_radius_y = radii.y - thickness;

	float current_angle = begin_angle;
	float final_angle = begin_angle + total_angle;

	float delta_angle = (2*M_PI / quality); 
	gs::Vec2f offset = {float(center.x), float(center.y)};

	std::vector<SDL_Vertex> vertices;
	SDL_Vertex common_fields;
		common_fields.color = col;
		common_fields.tex_coord.x = 1;
		common_fields.tex_coord.y = 1;

	int count = 0;
	bool break_next = false;
	gs::Vec2f prev_border_out;
	gs::Vec2f prev_border_in;

	if(not fill)
		goto _not_fill;

	while(current_angle <= final_angle)
	{
		gs::Vec2f border_out;
		border_out.x = std::cos(current_angle) * radii.x; 
		border_out.y = std::sin(current_angle) * radii.y; 

		gs::Vec2f border_in;
		border_in.x = std::cos(current_angle) * min_radius_x; 
		border_in.y = std::sin(current_angle) * min_radius_y; 

		border_out += offset;
		border_in += offset;

		if(count > 0) // = ce n'est pas le premier tour de boucle.
		{
			// Triangle 1.
			vertices.push_back(common_fields);
				vertices.back().position.x = prev_border_out.x;
				vertices.back().position.y = prev_border_out.y;

			vertices.push_back(common_fields);
				vertices.back().position.x = prev_border_in.x;
				vertices.back().position.y = prev_border_in.y;

			vertices.push_back(common_fields);
				vertices.back().position.x = border_out.x;
				vertices.back().position.y = border_out.y;

			// Triangle 2.
			vertices.push_back(common_fields);
				vertices.back().position.x = prev_border_in.x;
				vertices.back().position.y = prev_border_in.y;

			vertices.push_back(common_fields);
				vertices.back().position.x = border_out.x;
				vertices.back().position.y = border_out.y;

			vertices.push_back(common_fields);
				vertices.back().position.x = border_in.x;
				vertices.back().position.y = border_in.y;
			

		}
		
		count+=1;

		prev_border_out = border_out;
		prev_border_in = border_in;

		if(break_next) break;

		if((current_angle + delta_angle) > final_angle)
		{
			current_angle = final_angle;
			break_next = true;
		}
		else
			current_angle += delta_angle;
	}

	
	SDL_RenderGeometry(ren, nullptr, vertices.data(), int(vertices.size()), nullptr, 0);
	

	_not_fill:
	if(not fill)
	{
		//Ellipse externe.
		drawEllipse(ren, center, radii, col, false, total_angle, begin_angle);
		//Ellipse interne.
		drawEllipse(ren, center, {int(min_radius_x), int(min_radius_y)}, col, false, total_angle, begin_angle);
		
		//La fermeture des bords, si nécessaire.
		if(total_angle < 2*_pi_)
		{
			Vec2f pt1, pt2;
			pt1.x = std::cos(begin_angle)*min_radius_x +offset.x;
			pt1.y = std::sin(begin_angle)*min_radius_y +offset.y;
			pt2.x = std::cos(begin_angle)*radii.x +offset.x;
			pt2.y = std::sin(begin_angle)*radii.y +offset.y;
			drawLine(ren, pt1.to<int>(), pt2.to<int>(), col);

			pt1.x = std::cos(final_angle)*min_radius_x +offset.x;
			pt1.y = std::sin(final_angle)*min_radius_y +offset.y;
			pt2.x = std::cos(final_angle)*radii.x +offset.x;
			pt2.y = std::sin(final_angle)*radii.y +offset.y;
			drawLine(ren, pt1.to<int>(), pt2.to<int>(), col);
		}
	}

	return true;
}




bool drawCircle(SDL_Renderer* ren, const Vec2i& center, int radius, const SDL_FColor& col, const bool& fill, float total_angle, float begin_angle)
{
	bool success = drawEllipse(ren, center, {radius, radius}, col, fill, total_angle, begin_angle);

	if(not success)
		gs::lastError(gs::str("from 'drawCircle()' : ")+gs::lastError());
	return success;
}

bool drawCircleThickness(SDL_Renderer* ren, const Vec2i& center, int radius, int thickness, const SDL_FColor& col, const bool& fill, float total_angle, float begin_angle)
{
	bool success = drawEllipseThickness(ren, center, {radius, radius}, thickness, col, fill, total_angle, begin_angle);

	if(not success)
		gs::lastError(gs::str("from 'drawCircleThickness()' : ")+gs::lastError());
	return success;
}	












bool drawRect(SDL_Renderer* ren, const SDL_FRect& rect, const SDL_FColor* col, const bool& fill, SDL_BlendMode* b_mode, const Rotation_P& rotation)
{
	return drawRect(ren, &rect, col, fill, b_mode, rotation);
}
bool drawRect(SDL_Renderer* ren, const SDL_FRect* rect, const SDL_FColor& col, const bool& fill, SDL_BlendMode* b_mode, const Rotation_P& rotation)
{
	return drawRect(ren, rect, &col, fill, b_mode, rotation);
}
bool drawRect(SDL_Renderer* ren, const SDL_FRect& rect, const SDL_FColor& col, const bool& fill, SDL_BlendMode* b_mode, const Rotation_P& rotation)
{
	return drawRect(ren, &rect, &col, fill, b_mode, rotation);
}







bool drawShape(SDL_Renderer* ren, Shape_P s)
{
	if(!ren) 
	{
		gs::lastError("||Error|| in 'drawShape()', invalid renderer. ");
		return false;
	}

	bool success = true;

	auto _X = [&](const float& percent) {
		return s.area.x + s.area.w*percent;
	};
	auto _Y = [&](const float& percent) {
		return s.area.y + s.area.h*percent;
	};
	auto _W = [&](const float& percent) {
		return s.area.w*percent;
	};
	auto _H = [&](const float& percent) {
		return s.area.h*percent;
	};


	SDL_BlendMode* bm=nullptr;
	if(s.set_blend_mode)
		bm = &s.blend_mode;

	// if(s.thickness != 1)
	// {
	// 	float prev = s.thickness;
	// 	s.thickness = _W(s.thickness);

	// 	if(s.thickness == 0 and prev != 0)
	// 		s.thickness = 1;
	// }


	if(s.type == Shape_P::E) //Ellipse.
	{
		Vec2i center={static_cast<int>(_X(s.pos.x)), static_cast<int>(_Y(s.pos.y))};
		Vec2i radii={static_cast<int>(_W(s.size.x)), static_cast<int>(_H(s.size.y))};

		if(s.thickness == 0)
		{
			success = success & drawEllipse(ren, center, radii, s.color, s.fill, s.total_angle, s.begin_angle);
		}
		else
		{
			s.thickness = _W(s.thickness);

			if(s.thickness == 0)
				success = success & drawEllipse(ren, center, radii, s.color, false, s.total_angle, s.begin_angle);
			else
				success = success & drawEllipseThickness(ren, center, radii, int(s.thickness), s.color, s.fill, s.total_angle, s.begin_angle);
		}


		// if(s.thickness != 1)
		// {
		// 	if(s.thickness != 0)
		// 		success = success & drawEllipseThickness(ren, center, radii, int(s.thickness), s.color, s.fill, s.total_angle, s.begin_angle);
		// 	else
		// 		success = success & drawEllipse(ren, center, radii, s.color, false, s.total_angle, s.begin_angle);
		// }
		// else
		// 	success = success & drawEllipse(ren, center, radii, s.color, s.fill, s.total_angle, s.begin_angle);
	}
	else
	if(s.type == Shape_P::R) //Rectangle.
	{
		SDL_FRect r={_X(s.pos.x), _Y(s.pos.y), _W(s.size.x), _H(s.size.y)};

		if(s.round_percent != 0)
			success = success & drawRoundedRect(ren, r, s.round_percent, s.color, s.fill, bm);
		else
		if(s.rot_ang != 0)
			success = success & drawRect(ren, &r, &s.color, s.fill, bm, {s.rot_ang, s.rot_pt, s.clockwise});
		else
			success = success & drawRect(ren, &r, &s.color, s.fill, bm);
	}
	else
	if(s.type == Shape_P::T) //Triangle.
	{
		Triangle t ={{float(_X(s.pos.x)), float(_Y(s.pos.y))}, 
					{float(_X(s.pos2.x)), float(_Y(s.pos2.y))}, 
					{float(_X(s.pos3.x)), float(_Y(s.pos3.y))}};

		if(s.rot_ang != 0)
		{
			Vec2f rot_center;
			if(s.rot_pt == gs::Vec2f(0,0))
				rot_center = t.getCenter();
			else
				rot_center = gs::Vec2f(_X(s.rot_pt.x), _Y(s.rot_pt.y));

			t.rotate((double)s.rot_ang, &rot_center, s.clockwise);
		}

		success = success & t.draw(ren, &s.color, s.fill, bm);
	}
	else
	if(s.type == Shape_P::S) //Segment.
	{
		Vec2i pt1 = {static_cast<int>(_X(s.pos.x)), static_cast<int>(_Y(s.pos.y))};
		Vec2i pt2 = {static_cast<int>(_X(s.pos2.x)), static_cast<int>(_Y(s.pos2.y))};

		if(s.rot_ang != 0)
		{
			Vec2i rot_center = {static_cast<int>(_X(s.rot_pt.x)), static_cast<int>(_Y(s.rot_pt.y))};

			pt1 = rotatePoint(rot_center.x, rot_center.y, pt1.x, pt1.y, s.rot_ang, s.clockwise).to<int>();
			pt2 = rotatePoint(rot_center.x, rot_center.y, pt2.x, pt2.y, s.rot_ang, s.clockwise).to<int>();
		}

		success = success & drawLine(ren, pt1, pt2, s.color, bm);
	}


	if(not success)
		gs::lastError(gs::str("from 'drawShape()' : ")+gs::lastError());

	return success;
}


}//namespace gs
