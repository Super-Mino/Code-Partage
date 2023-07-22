#include "SDL2_GSlib-Entity.hpp"
#include <SDL2/SDL_image.h> 
//version modifié le 22/11/22 (à moins d'oubli de modifier cette ligne)

namespace sm
{
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//____________________________LES FONCTIONS DE BODY_________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________


Body::Body()
: angle_(0.0), /*vecteurX(0), vecteurY(0),*/ m_dest_decalage_x(0), m_dest_decalage_y(0) 
{
    src = {0,0,0,0};
    // hitbox_.rect_nb = 1;
    dest = {10,10,32,32};
    image = nullptr;  
};


//__________________________________________________________________________________________
//__________________________________________________________________________________________


Body::~Body() 
{}


//__________________________________________________________________________________________
//__________________________________________________________________________________________


void Body::initHitbox(int x, int y, int w, int h)
{
    m_support_R_pos_x = x;
    m_support_R_pos_y = y;

    hitbox_.main_rect.x = x; hitbox_.main_rect.y = y; 
    hitbox_.main_rect.w = w; hitbox_.main_rect.h = h;

    dest = hitbox_.main_rect;
    
    dest.x = hitbox_.main_rect.x - m_dest_decalage_x; dest.y = hitbox_.main_rect.y - m_dest_decalage_y;
}



//__________________________________________________________________________________________
//__________________________________________________________________________________________



void Body::setDestRectDecalage(int x, int y)
{
    dest.x = hitbox_.main_rect.x + x; dest.y = hitbox_.main_rect.y + y;
    
    m_dest_decalage_x = x;         m_dest_decalage_y = y;
}



//__________________________________________________________________________________________
//__________________________________________________________________________________________



void Body::incrDestRectDecalage(int x, int y)
{
    m_dest_decalage_x += x;        m_dest_decalage_y += y;

    dest.x = hitbox_.main_rect.x + m_dest_decalage_x;
    dest.y = hitbox_.main_rect.y + m_dest_decalage_y;
}



//__________________________________________________________________________________________
//__________________________________________________________________________________________


void Body::setX(int _x) {hitbox_.main_rect.x = _x; }; 
        

void Body::setY(int _y) {hitbox_.main_rect.y = _y;};  


void Body::setXY(int _x, int _y) {setX(_x); setY(_y);} 
        

void Body::setW(unsigned int val) {hitbox_.main_rect.w = val;};
        

void Body::setH(unsigned int val) {hitbox_.main_rect.h = val;};


//__________________________________________________________________________________________
//__________________________________________________________________________________________


void Body::incrX(double _x)
{
    //encienne méthode qui causait un décalage lors des décrémentations
        // vecteurX += _x; 
        // if(vecteurX >= 1 || vecteurX <= -1)
        // {
        //     int partieEntiere = (int) vecteurX;  
        //     //incrementation de la valeur valable 
        //     hitbox_.main_rect.x += partieEntiere;  
        //     //retrait de la partie utilisée
        //     vecteurX -= (double) partieEntiere;
        // }

    m_support_R_pos_x += _x;
    hitbox_.main_rect.x = (int)m_support_R_pos_x;
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________


void Body::incrY(double _y)
{
    //encienne méthode qui causait un décalage lors des décrémentations
        // vecteurY += _y; 
        // if(vecteurY >= 1 || vecteurY <= -1)
        // {
        //     int partieEntiere = (int) vecteurY;  
        //     //incrementation de la valeur valable 
        //     hitbox_.main_rect.y += partieEntiere;  
        //     //retrait de la partie utilisée
        //     vecteurY -= (double) partieEntiere;
        // }

    m_support_R_pos_y += _y;
    hitbox_.main_rect.y = (int)m_support_R_pos_y;
}

//__________________________________________________________________________________________
//__________________________________________________________________________________________



void Body::incrXY(double _x, double _y) {incrX(_x); incrY(_y);} 


//__________________________________________________________________________________________
//__________________________________________________________________________________________



void Body::setAngle(double radian)
{
    double radian_absolu = (radian < 0 ? radian*(-1) : radian );

    while(radian_absolu >= 2.0*_pi_)
    {
        radian_absolu -= 2.0*_pi_;
    }

    radian = (radian < 0 ? radian_absolu * (-1) : radian_absolu );

    angle_ = radian; 
}  



//__________________________________________________________________________________________
//__________________________________________________________________________________________



void Body::incrAngle(double radian)
{
    angle_ += radian; 

    double angle_absolu = (angle_ < 0 ? angle_ * (-1) : angle_ );

    while(angle_absolu >= 2.0*_pi_)
    {
        angle_absolu -= 2.0*_pi_; 
    }
    angle_ = angle_absolu;
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________



bool Body::setImg(SDL_Renderer *ren, const char *path, bool destroy) 
{
    if(destroy)
    {
        if(image != nullptr)
        {
            SDL_DestroyTexture(image);
            image = nullptr;
        }     
    }
    else
        addToClearAtTheEnd(image);

    image = sm::loadImg(ren, path);

    if(image == nullptr)
    {
        sm::lastError(sm::str("from 'sm::Body::setImg()' : ")+ sm::lastError());
        return false;
    }
   
    return true;
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________

       

void Body::setImgRGB(SDL_Renderer *ren, const char *path, Color color_to_hide, bool destroy) 
{ 
    if(destroy)
    {
        if(image != nullptr)
        {
            SDL_DestroyTexture(image);
            image = nullptr;
        }     
    }
    else
        addToClearAtTheEnd(image);


    image = loadImgAndDeleteAColor(ren, path, color_to_hide);
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________



void Body::setTexture(SDL_Texture *tex, Game* g, bool destroy) 
{ 
    if(tex != nullptr)
    {   
        if(image != nullptr)
        {
            if(destroy)
            {
                SDL_DestroyTexture(image);
                image = nullptr;
            }
            else
                if(g != nullptr)
                {
                    g->addToClearAtTheEnd(image); 
                }
                else
                    addToClearAtTheEnd(image);
        }
            
        image = tex; 
    }
}


void Body::setSrcX(unsigned int val) {src.x = val;}; 


void Body::setSrcW(unsigned int val) {src.w = val;}; 


void Body::setSrcY(unsigned int val) {src.y = val;}; 


void Body::setSrcH(unsigned int val) {src.h = val;}; 


void Body::setSrc(SDL_Rect rec) {src = rec; }


void Body::setDestW(unsigned int val) {dest.w = val;};


void Body::setDestH(unsigned int val) {dest.h = val;};


void Body::setDestWH(unsigned int _w, unsigned int _h) { setDestW(_w); setDestH(_h); };

    
void Body::incrDestW(int val) { setDestW(getDestW()+val);};


void Body::incrDestH(int val) { setDestH(getDestH()+val);};
        

void Body::incrDestWH(int _w, int _h) { incrDestW(_w); incrDestH(_h); };


void Body::setState(const short val) {etat_ = val;};


void Body::setState2(const short val) {etat2_ = val;}


void Body::setFlip(const SDL_RendererFlip flip) {m_flip = flip;}


void Body::setShow(const bool val) {m_show = val;}


//__________________________________________________________________________________________
//__________________________________________________________________________________________



Hitbox& Body::getHitbox() {return hitbox_;}; 


SDL_Rect& Body::getHitboxRect() {return hitbox_.main_rect;}; 


Hitbox Body::getHitboxCpy() {return hitbox_;}; 


SDL_Rect Body::getHitboxRectCpy() {return hitbox_.main_rect;}; 


int Body::getX() const {return hitbox_.main_rect.x;}; 


double Body::getX_f() const {return m_support_R_pos_x;}//{return static_cast<double>(hitbox_.main_rect.x)+vecteurX;}


int Body::getW() const {return hitbox_.main_rect.w;}; 


int Body::getY() const {return hitbox_.main_rect.y;}; 


double Body::getY_f() const {return m_support_R_pos_y;}//{return static_cast<double>(hitbox_.main_rect.y)+vecteurY;}


int Body::getH() const {return hitbox_.main_rect.h;};


double Body::getAngle() const {return angle_;};


SDL_Rect* Body::getSrc() {return &src;}; 


int Body::getSrcX() const {return src.x;}; 


int Body::getSrcW() const {return src.w;}; 


int Body::getSrcY() const {return src.y;}; 


int Body::getSrcH() const {return src.h;};


SDL_Rect* Body::getDest() 
{ 
    dest.x = hitbox_.main_rect.x + m_dest_decalage_x; dest.y = hitbox_.main_rect.y + m_dest_decalage_y;
    return &dest;
}


int Body::getDestW() {return getDest()->w;};


int Body::getDestH() {return getDest()->h;};


int Body::getDestX() {return getDest()->x;};


int Body::getDestY() {return getDest()->y;};


short Body::getState() const {return etat_;};


short Body::getState2() const {return etat2_;};


SDL_RendererFlip Body::getFlip() const {return m_flip;}


bool Body::getShow() const {return m_show;}


SDL_Texture* Body::getImg() {return image;} 


SDL_Texture* Body::getTexture() {return image;} 


bool Body::isImgLoaded() const {if(image != nullptr) return true; return false;}



//__________________________________________________________________________________________
//__________________________________________________________________________________________



bool Body::selfRenderCopy(Game *g, const double angle_rad, const SDL_RendererFlip flip)
{
    if(not m_show)
        return true;

    dest.x = hitbox_.main_rect.x + m_dest_decalage_x; dest.y = hitbox_.main_rect.y + m_dest_decalage_y;

    SDL_RendererFlip _flip = flip;
    if(_flip == SDL_FLIP_NONE)
        _flip = m_flip;

    
    if(src.x >= 0 && src.y >= 0 && src.w > 0 && src.h > 0) //si le src est initialisé
    {
        if(SDL_RenderCopyEx(g->ren(), image, &src, &dest, degree(angle_rad), nullptr, _flip) != 0)
        {
            sm::lastError(str("\n||Warning|| in 'Body::selfRenderCopy()' : ")+str(SDL_GetError()));
            return false;
        }    
    }
    else 
    {
        if(SDL_RenderCopyEx(g->ren(), image, nullptr, &dest, degree(angle_rad), nullptr, _flip) != 0)
        {
            sm::lastError(str("\n||Warning|| in 'Body::selfRenderCopy()' : ")+str(SDL_GetError()));
            return false;
        }
    }

    return true; 
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________



bool Body::selfRenderCopy(Game *g)
{
    return selfRenderCopy(g, angle_, m_flip);
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________



bool Body::selfRenderCopy(Game *g, const SDL_RendererFlip flip)
{
    return selfRenderCopy(g, angle_, flip);
}

//__________________________________________________________________________________________
//__________________________________________________________________________________________


void Body::drawHitbox(Game *g, SDL_Color color, bool fill) 
{
    if(not g)
    {
        sm::lastError("||Warning|| in 'sm::Body::drawHitbox()', given `sm::Game' is nullptr. ");
        return;
    }

    SDL_Color previousC;
    SDL_GetRenderDrawColor(g->ren(), &previousC.r,&previousC.g,&previousC.b,&previousC.a);
    SDL_SetRenderDrawColor(g->ren(), color.r,color.g,color.b,color.a);
    hitbox_.drawMainRect(g, fill);
    SDL_SetRenderDrawColor(g->ren(), previousC.r,previousC.g,previousC.b,previousC.a);
}

//__________________________________________________________________________________________
//__________________________________________________________________________________________



bool Body::isCollision( Body &body, int marge_dIgnorence_a, int marge_dIgnorence_b) const
{
    return ::sm::isCollisionBetweenTwoHitboxes(hitbox_, body.getHitbox(),
        marge_dIgnorence_a, marge_dIgnorence_b);

    // int mia = marge_dIgnorence_a; //sert à des fois ne pas prendre la colision au pixel près
    // int mib = marge_dIgnorence_b; //'mia' pour le `Body' qui appel la fonction, et 'mib' l'autre (explication du nom : m->marge i->ignorence a ou b)

    // SDL_Rect selfbox = hitbox_.main_rect;  // a
    // SDL_Rect *tmp = body.getHitboxRect();
    // SDL_Rect otherbox = *tmp;// b
    //                                                         //explication #0
    
    // //Prise des intervalles sur l'axe des abscisses (le "x E [x1 ; x2]" )
    // int a_x1 = selfbox. x; int a_x2 = a_x1 + selfbox. w -1; //explication #1
    // int b_x1 = otherbox.x; int b_x2 = b_x1 + otherbox.w -1;

    // //Prise des intervalles sur l'axe des ordonnées
    // int a_y1 = selfbox. y; int a_y2 = a_y1 + selfbox. h -1;
    // int b_y1 = otherbox.y; int b_y2 = b_y1 + otherbox.h -1;

    // //prise en compte de la marge d ignorence mais cepenendant ne doit pas annuler l'hitbox complètement
    // if(mia*2 > (a_x2 - a_x1 -2)) mia = (a_x2 - a_x1 -2) /2;
    // if(mib*2 > (b_x2 - b_x1 -2)) mib = (b_x2 - b_x1 -2) /2;
    // if(mia*2 > (a_y2 - a_y1 -2)) mia = (a_y2 - a_y1 -2) /2;
    // if(mib*2 > (b_y2 - b_y1 -2)) mib = (b_y2 - b_y1 -2) /2;

    // a_x1 += mia; a_x2 -= mia;    a_y1 += mia; a_y2 -= mia;
    // b_x1 += mib; b_x2 -= mib;    b_y1 += mib; b_y2 -= mib;


    // //vérification qu'ils ne cognent pas sur les x
    // if( a_x2 < b_x1 || b_x2 < a_x1)                         //explications #2
    // {
    //     return false; //car pas de collision puisque au moins une coditions n'est pas remplie
    // }
    //     // Si l'on arrive ici c'est qu'ils se touchent sur les x. 
    //     // Donc on vérifie s'ils se touchent AUSSI sur les y.
    // else if( a_y2 < b_y1 || b_y2 < a_y1)                    //explications #3
    // {
    //     return false; 
    // }
    // else
    // {
    //     // Si on atteint ce stade, c est que : 
    //        // ni ("ne se touchent pas en x")    ni ("ne se touchent pas en y")
    //        // Autrement dit ils se touchent en x ET en y donc il y a collision
    //     return true;
    // }
        
    // return false; //par sécurité (on sait jamais) 


    // /*----explications-----------------------------------

    //     #0  Il n'y a collision QUE si leurs DEUX intervalles (aux "body"), sur les axes x ET y, se touchent

    //     #1  -1 car les indices des pixels commencent à 0. 
    //         Si x est à 0 et que w vaut 4, x + w = 4, or "4" est l indice du "cinquieme" pixel et nous 
    //         n'occupons que les 4 premiers pixels, dont les indices sont "0","1","2" et "3". Le dernier 
    //         indice est donc 3, d'où le (-1)

    //     #2  si le a est (totalement à la gauche de b) OU (totalement à sa droite)

    //     #3  si le a est (totalement en haut de b) OU (totalement en bas de b)

    //     ps: peut importe qui est a, qui est b; si l'un touche l'autre, l'autre touche l'un
    // */
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________

bool Body::willHit(Hitbox& hitbox, double self_move_x, double self_move_y, 
                     int marge_dIgnorence_a, 
                     int marge_dIgnorence_b, 
                     int* dist_x_before_hit__out,
                     int* dist_y_before_hit__out) const

{
    int futur_x = getX_f() + self_move_x;
    int futur_y = getY_f() + self_move_y; 

    Hitbox selfbox_cpy = hitbox_;

    selfbox_cpy.main_rect.x = futur_x;
    selfbox_cpy.main_rect.y = futur_y;

    bool res = isCollisionBetweenTwoHitboxes(selfbox_cpy, hitbox,
        marge_dIgnorence_a, marge_dIgnorence_b);

    if(res)
    {
        if(dist_x_before_hit__out)
            *dist_x_before_hit__out = distOnX(hitbox_, hitbox);

        if(dist_y_before_hit__out)
            *dist_y_before_hit__out = distOnY(hitbox_, hitbox);
    }
    else
    {
        if(dist_x_before_hit__out)
            *dist_x_before_hit__out = 0;

        if(dist_y_before_hit__out)
            *dist_y_before_hit__out = 0;
    }
    
    return res;
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________


bool Body::willHit(Body& body, double self_move_x, double self_move_y, 
                     int marge_dIgnorence_a, 
                     int marge_dIgnorence_b,
                     int* dist_x_before_hit__out,
                     int* dist_y_before_hit__out) const
{

   return willHit(body.getHitbox(), self_move_x, self_move_y, 
                marge_dIgnorence_a, marge_dIgnorence_b,
                dist_x_before_hit__out, dist_y_before_hit__out);
}

































//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//____________________________LES FONCTIONS DE ENTITY_______________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________


Entity::Entity(std::string _name, short nb_member)
: name_(_name),    nb_mb(nb_member) 
{   
    if(nb_member > 1)
       
        for(int count = 1; count < nb_mb; ++count)
        {
            Body b;
            member_.push_back(b);
        }

    if(show_comments)
    std::cout << "\tEntity \"" << _name << "\" with " << nb_member << " member(s) is created" << std::endl;
    
    inputUP(false); inputDOWN(false); inputLEFT(false); inputRIGHT(false);
} 


//__________________________________________________________________________________________
//__________________________________________________________________________________________



Entity::~Entity()
{
    if(show_comments)
    std::cout << "\tEntity \""<<name_<< "\" is succefuly deleted" << std::endl; 
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________


void Entity::setName(std::string _n) {name_ = _n;}


void Entity::inputUP(bool i) {input_.up = i;};


void Entity::inputDOWN(bool i) {input_.down = i;};


void Entity::inputLEFT(bool i) {input_.left = i;};


void Entity::inputRIGHT(bool i) {input_.right = i;}; 


void Entity::setSpeed(double val) {speed_ = val;}


// void Entity::setState(const short val) {etat_ = val;}
        
        
// void Entity::setState2(const short val) {etat2_ = val;}



//__________________________________________________________________________________________
//__________________________________________________________________________________________




std::string Entity::getName() const {return name_;}


short Entity::getNbOfMembers() const {return nb_mb;}


bool Entity::inputUP() const {return input_.up; }


bool Entity::inputDOWN() const {return input_.down; }


bool Entity::inputLEFT() const {return input_.left; }


bool Entity::inputRIGHT() const {return input_.right; }


double Entity::getSpeed() const {return speed_;};


// short Entity::getState() const {return etat_;};

        
// short Entity::getState2() const {return etat2_;};


//__________________________________________________________________________________________
//__________________________________________________________________________________________




void Entity::input(std::string btn, bool i)
{

    if(btn == "space") input_.space = i; else if(btn == "a") input_.a = i;   else if(btn == "b") input_.b = i;
    else if(btn == "y") input_.y = i;    else if(btn == "x") input_.x = i;   else if(btn == "r") input_.r = i;
    else if(btn == "l") input_.l = i;    else if(btn == "zr") input_.zr = i; else if(btn == "zl") input_.zl = i;
    else if(btn == "start") input_.start = i;
}

//------------------------------------------------------------------------------------

bool Entity::input(std::string btn) const
{
    if(btn == "space") return input_.space; else if(btn == "a")  return input_.a; else if(btn == "b")   return input_.b;
    else if(btn == "y") return input_.y;    else if(btn == "x")  return input_.x; else if(btn == "r")   return input_.r;
    else if(btn == "l") return input_.l;    else if(btn == "zr") return input_.zr; else if(btn == "zl") return input_.zl;
    else if(btn == "start") return input_.start;
    else return false;
}



//__________________________________________________________________________________________
//__________________________________________________________________________________________


Body& Entity::member(unsigned int i) 
{ 
    if(i == 0)
        return *this;

    if(nb_mb == 0) return *this;

    i --;

    if(i >= nb_mb-1) return *this; 
    return member_[i]; 
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________


void Entity::moveAllBodies( Axis axe,  Signe signe, double _dt)
{
    double speed = speed_;
    switch(signe)
    {
        case POSITIVE : /*speed = speed;*/    break;
        case NEGATIVE : speed = speed*-1; break;

        default : lastError("||Warning|| in function `moveAllBodies()' of '"+name_+"', invalid parameter (no2) `signe'.  "); 
            return;
    }

    moveAllBodies(axe, speed, _dt);
    
}


//------------------------------------------------------------------------------------


void Entity::moveAllBodies( Axis axe, double speed_val, double _dt)
{
    if(axe == Y_AXIS)
    {
        for(int i{0}; i< nb_mb; i++)
        {
            member(i).incrY(speed_val *_dt);
        }
    }


    else if(axe == X_AXIS)
    {
        for(int i{0}; i< nb_mb; i++)
        {
            member(i).incrX(speed_val *_dt);
        }
    }
}



//__________________________________________________________________________________________
//__________________________________________________________________________________________


void Entity::selfRenderCopyAll(Game *g)
{
    for( int i = 0; i < nb_mb; i++)
    {
        member(i).selfRenderCopy(g); 
    }
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________



void Entity::selfRenderCopyAll(Game *g, const double radian, SDL_RendererFlip flip)
{
    for( int i = 0; i < nb_mb; i++)
    {
        member(i).selfRenderCopy(g, radian, flip); 
    }
}
        

//__________________________________________________________________________________________
//__________________________________________________________________________________________



void Entity::selfRenderCopyAll(Game *g, SDL_RendererFlip flip)
{
    for( int i = 0; i < nb_mb; i++)
    {
        member(i).selfRenderCopy(g, flip); 
    }
}

//__________________________________________________________________________________________
//__________________________________________________________________________________________


bool Entity::isCollision( Entity &entity, int marge_dIgnorence_a, int marge_dIgnorence_b)
{
    int max2 = entity.getNbOfMembers();


    for(    int i{0};   i< nb_mb;    i++)
    {
        for(int j{0};   j < max2;   j++)
        {
            if(member(i).isCollision(entity.member(j), marge_dIgnorence_a, marge_dIgnorence_b))
                return true;
        }
    }
    return false;
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________


bool Entity::isCollision( Body &body, int marge_dIgnorence_a, int marge_dIgnorence_b)
{

    for( int i{0};  i<  nb_mb;  i++)
    {
        if(member(i).isCollision(body, marge_dIgnorence_a, marge_dIgnorence_b))
            return true;
    }
    return false;
}


//__________________________________________________________________________________________
//__________________________________________________________________________________________





































//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//____________________________LES FONCTIONS HORS CLASSE_____________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________
//__________________________________________________________________________________________


bool isCollisionBetweenTwoHitboxes(const Hitbox& a, const Hitbox& b, 
                int marge_dIgnorence_a , int marge_dIgnorence_b )
{
    /*
    Cette fonction teste si deux hitbox sont l'un DANS l'autre. 
    Ici collé = pas de collision. 

    sm::Hitbox est fait avec des valeurs entière pour respecter l'organisation des pixels
    d'un écran. En effet, si un sm::Body est à 3.6, sa position à l'écran sera au pixel 
    de l'index 3 (peu importe l'axe), et c'est cette valeur de position qu'a son hitbox 
    et qui sera prise en compte dans le traitement des collisions.
    
    Les marges d'ignorance servent à agrandir ou réduire la zone de traitement.
    Une marge négative va agrandir la surface de l'hitbox
    e.g avec -3 : si position x = 15, le teste va considérer jusqu'à 3 rangs avant, c'est
    à dire que va prendre en compte à partir de 12 bien que l'hitbox n'est qu'au 15, et 3 rangs
    après le coin opposé (le x+w-1).
    */

    int mia = marge_dIgnorence_a; 
    int mib = marge_dIgnorence_b; 

    SDL_Rect selfbox = a.main_rect;  // a
    SDL_Rect otherbox = b.main_rect; // b
                                                            //explication #0
    
    //Prise des intervalles sur l'axe des abscisses 
    int a_x1 = selfbox. x; int a_x2 = a_x1 + selfbox. w -1; //explication #1
    int b_x1 = otherbox.x; int b_x2 = b_x1 + otherbox.w -1;

    //Prise des intervalles sur l'axe des ordonnées
    int a_y1 = selfbox. y; int a_y2 = a_y1 + selfbox. h -1;
    int b_y1 = otherbox.y; int b_y2 = b_y1 + otherbox.h -1;

    //prise en compte de la marge d ignorence mais cependant ne doit pas annuler l'hitbox complètement
    if(mia*2 > (a_x2 - a_x1 -2)) mia = (a_x2 - a_x1 -2) /2; //le -2 est une marge aussi
    if(mib*2 > (b_x2 - b_x1 -2)) mib = (b_x2 - b_x1 -2) /2;
    if(mia*2 > (a_y2 - a_y1 -2)) mia = (a_y2 - a_y1 -2) /2;
    if(mib*2 > (b_y2 - b_y1 -2)) mib = (b_y2 - b_y1 -2) /2;

    a_x1 += mia; a_x2 -= mia;    a_y1 += mia; a_y2 -= mia;
    b_x1 += mib; b_x2 -= mib;    b_y1 += mib; b_y2 -= mib;



    //vérification qu'ils ne cognent pas sur les x
    if( a_x2 < b_x1 || b_x2 < a_x1)                         //explications #2
    {
        return false; //car pas de collision puisque au moins une coditions n'est pas remplie
    }
        // Si l'on arrive ici c'est qu'ils se touchent sur les x. 
        // Donc on vérifie s'ils se touchent AUSSI sur les y.
    else if( a_y2 < b_y1 || b_y2 < a_y1)                    //explications #3
    {
        return false; 
    }
    else
    {
        // Si on atteint ce stade, c est que : 
           // ni ("ne se touchent pas en x")    ni ("ne se touchent pas en y")
           // Autre dit ils se touchent en x ET en y donc y a collision
        return true;
    }
        
    return false; //par sécurité (on sait jamais) 


    /*----explications-----------------------------------

        #0  Il n'y a collision QUE si leurs DEUX intervalles (aux rectangles), sur les axes x et y, se touchent.

        #1  -1 car les indices des pixels commencent à 0. 
            Si x est à 0 et que w vaut 4, x + w = 4, or "4" est l'indice du "cinquième" pixel et nous 
            n'occupons que les 4 premiers pixels, dont les indices sont "0", "1", "2", "3". Le dernier 
            indice est donc 3, d'où le (-1).

        #2  si le a est (totalement à la gauche de b) OU (totalement à sa droite)

        #3  si le a est (totalement en haut de b) OU (totalement en bas de b)

        Note : peut importe qui est a, qui est b; si l'un touche l'autre, l'autre touche l'un.
    */
}
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//

Hitbox hitboxOf(int x, int y, int w, int h)
{
    Hitbox box;
    box.main_rect.x = x;  box.main_rect.y = y;  box.main_rect.w = w;  box.main_rect.h = h;
    return box;
}
        //----------------------------------------------------------//
        //----------------------------------------------------------//

Hitbox hitboxOf(Body &body)
{
    return body.getHitbox();
}


bool willHit(Body& body1, double body1_move_x, double body1_move_y, 
             Body& body2, double body2_move_x, double body2_move_y)
{
    Hitbox 
        box1_cpy = body1.getHitbox(), 
        box2_cpy = body2.getHitbox();

    box1_cpy.main_rect.x = body1.getX_f() + body1_move_x;
    box1_cpy.main_rect.y = body1.getY_f() + body1_move_y;

    box2_cpy.main_rect.x = body2.getX_f() + body2_move_x;
    box2_cpy.main_rect.y = body2.getY_f() + body2_move_y;

    return ::sm::isCollisionBetweenTwoHitboxes(box1_cpy, box2_cpy);
}



unsigned int distOnX(const Hitbox& hitbox1, const Hitbox& hitbox2)
{
    int x1=0, x2=0;
    /*si 1 est à gauche, 2 à droite :
       [1]------[2] 
       --- = distance retournée
    */ 
    if(hitbox1.main_rect.x < hitbox2.main_rect.x)
    {
        x1 = hitbox1.main_rect.x + hitbox1.main_rect.w; //ici pas de w - 1
        x2 = hitbox2.main_rect.x;
    }
    /*si 2 est à gauche, 1 à droite :
       [2]------[1] 
       --- = distance retournée
    */ 
    else
    if(hitbox2.main_rect.x < hitbox1.main_rect.x)
    {
        x1 = hitbox2.main_rect.x + hitbox2.main_rect.w; 
        x2 = hitbox1.main_rect.x;
    }
    else
    if(hitbox2.main_rect.x == hitbox1.main_rect.x)
    {
        return 0;
    }
    else //Normalement ne devrait jamais arriver ici.
    {
        sm::lastError("||Warning|| in 'sm::distOnX()', impossible to determine the distance. ");
        return 0;
    }

    int res = x2 - x1;

    if(res < 0) //= il y'a intersection sur cette axe, donc il n'y'a pas vraiment de distance entre les deux rectangles.
        return 0; 

    return res;

}



unsigned int distOnY(const Hitbox& hitbox1, const Hitbox& hitbox2)
{
    int y1=0, y2=0;
    /*si 1 est en haut, 2 en bas :
       [1]
       -
       -
       -
       [2] 
       --- = distance retournée
    */ 
    if(hitbox1.main_rect.y < hitbox2.main_rect.y)
    {
        y1 = hitbox1.main_rect.y + hitbox1.main_rect.h; //ici pas de h - 1
        y2 = hitbox2.main_rect.y;
    }
    /*si 2 est en haut, 1 en bas :
       [2]
       -
       -
       -
       [1] 
       --- = distance retournée
    */ 
    else
    if(hitbox2.main_rect.y < hitbox1.main_rect.y)
    {
        y1 = hitbox2.main_rect.y + hitbox2.main_rect.h; 
        y2 = hitbox1.main_rect.y;
    }
    else
    if(hitbox2.main_rect.y == hitbox1.main_rect.y)
    {
        return 0;
    }
    else //Normalement ne devrait jamais arriver ici.
    {
        sm::lastError("||Warning|| in 'sm::distOnY()', impossible to determine the distance. ");
        return 0;
    }

    int res = y2 - y1;

    if(res < 0) //= il y'a intersection sur cette axe, donc il n'y'a pas vraiment de distance entre les deux rectangles.
        return 0; 

    return res;
}



unsigned int distOnX(Body& body1, Body& body2)
{
    return distOnX(body1.getHitbox(), body2.getHitbox());
}


unsigned int distOnY(Body& body1, Body& body2)
{
    return distOnY(body1.getHitbox(), body2.getHitbox());
}




double euclideanDist(const Hitbox& hitbox1, const Hitbox& hitbox2, en::Corner corner)
{
    int x1=0, y1=0, x2=0, y2=0;

    switch(corner)
    {
        case en::TOP_LEFT : 
            x1 = hitbox1.main_rect.x;
            y1 = hitbox1.main_rect.y;
            x2 = hitbox2.main_rect.x;
            y2 = hitbox2.main_rect.y;
            break;

        case en::TOP_RIGHT :
            x1 = hitbox1.main_rect.x+hitbox1.main_rect.w-1;
            y1 = hitbox1.main_rect.y;
            x2 = hitbox2.main_rect.x+hitbox2.main_rect.w-1;
            y2 = hitbox2.main_rect.y;  
            break;

        case en::DOWN_LEFT :
            x1 = hitbox1.main_rect.x;
            y1 = hitbox1.main_rect.y+hitbox1.main_rect.h-1;
            x2 = hitbox2.main_rect.x;
            y2 = hitbox2.main_rect.y+hitbox2.main_rect.h-1;
            break;

        case en::DOWN_RIGHT :
            x1 = hitbox1.main_rect.x+hitbox1.main_rect.w-1;
            y1 = hitbox1.main_rect.y+hitbox1.main_rect.h-1;
            x2 = hitbox2.main_rect.x+hitbox2.main_rect.w-1;
            y2 = hitbox2.main_rect.y+hitbox2.main_rect.h-1;
            break;

        case en::MIDDLE : 
            x1 = hitbox1.main_rect.x + hitbox1.main_rect.w/2;
            y1 = hitbox1.main_rect.y + hitbox1.main_rect.h/2;
            x2 = hitbox2.main_rect.x + hitbox2.main_rect.w/2;
            y2 = hitbox2.main_rect.y + hitbox2.main_rect.h/2;
            break;

        default :
            sm::lastError("||Warning|| in 'euclideanDist()', invalid corner. ");
            return 0;
    }

    return sm::euclideanDist(double(x1), double(y1), double(x2), double(y2));
}



double euclideanDist(Body& body1, Body& body2, en::Corner corner)
{
    return sm::euclideanDist(body1.getHitbox(), body2.getHitbox(), corner);
}



bool touchOnX(const Hitbox& hitbox1, const Hitbox& hitbox2)
{
    int x1 = hitbox1.main_rect.x;
    int xx1 = x1 + hitbox1.main_rect.w-1;

    int x2 = hitbox2.main_rect.x;
    int xx2 = x2 + hitbox2.main_rect.w-1;

    return 
    ( xx1+1 == x2 or 
        xx2+1 == x1 );
}


bool touchOnY(const Hitbox& hitbox1, const Hitbox& hitbox2)
{
    int y1 = hitbox1.main_rect.y;
    int yy1 = y1 + hitbox1.main_rect.h-1;

    int y2 = hitbox2.main_rect.y;
    int yy2 = y2 + hitbox2.main_rect.h-1;

    return 
    ( yy1+1 == y2 or 
        yy2+1 == y1 );
}


bool touchOnX(Body& body1, Body& body2)
{
    return touchOnX(body1.getHitbox(), body2.getHitbox());
}


bool touchOnY(Body& body1, Body& body2)
{
    return touchOnY(body1.getHitbox(), body2.getHitbox());
}


bool fstTouchSndSUp(const Hitbox& fst, const Hitbox& snd)
{
    if(touchOnY(fst, snd))
    {
        if(fst.main_rect.y < snd.main_rect.y)
        {
            //Puis vérifions que ça touche réelement. 
            //Note : touchOnY() vérifie seulement s'ils sont adjacents sur l'axe des ordonnés
            //et fstTouchSndSUp() vérifie si 'fst' touche réelement le côté haut de 'snd'.
            int x1 = fst.main_rect.x;
            int xx1 = x1 + fst.main_rect.w -1;

            int x2 = snd.main_rect.x;
            int xx2 = x2 + snd.main_rect.w -1;

            //Si un des côté de l'un (gauche et/ou droit) est dans l'interval des x qu'occupe l'autre 
            //alors ils se touchent en x et vient compléter la dernière des 3 conditions requises : 
            //être adjacent sur les y, 'fst' au dessus de 'snd' et  en collision sur les x.
            if(sm::isInInterval_in_in(x2, xx2, x1) or sm::isInInterval_in_in(x2, xx2, xx1))
                return true;
            if(sm::isInInterval_in_in(x1, xx1, x2) or sm::isInInterval_in_in(x1, xx1, xx2))
                return true;
            //Les autres côtés suivent les mêmes principes.
        }
    }

    return false;
}

bool fstTouchSndSDown(const Hitbox& fst, const Hitbox& snd)
{
    return fstTouchSndSUp(snd, fst);
}


bool fstTouchSndSLeft(const Hitbox& fst, const Hitbox& snd)
{
    if(touchOnX(fst, snd))
    {
        if(fst.main_rect.x < snd.main_rect.x)
        {
            int y1 = fst.main_rect.y;
            int yy1 = y1 + fst.main_rect.h -1;

            int y2 = snd.main_rect.y;
            int yy2 = y2 + snd.main_rect.h -1;

            if(sm::isInInterval_in_in(y2, yy2, y1) or sm::isInInterval_in_in(y2, yy2, yy1))
                return true;
            if(sm::isInInterval_in_in(y1, yy1, y2) or sm::isInInterval_in_in(y1, yy1, yy2))
                return true;
        }
    }

    return false;
}

bool fstTouchSndSRight(const Hitbox& fst, const Hitbox& snd)
{
    return fstTouchSndSLeft(snd, fst);
}



bool fstTouchSndSUp(Body& fst, Body& snd)
{
    return fstTouchSndSUp(fst.getHitbox(), snd.getHitbox());
}


bool fstTouchSndSDown(Body& fst, Body& snd)
{
    return fstTouchSndSDown(fst.getHitbox(), snd.getHitbox());
}


bool fstTouchSndSLeft(Body& fst, Body& snd)
{
    return fstTouchSndSLeft(fst.getHitbox(), snd.getHitbox());
}


bool fstTouchSndSRight(Body& fst, Body& snd)
{
    return fstTouchSndSRight(fst.getHitbox(), snd.getHitbox());
}











} // fin namespace sm