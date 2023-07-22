#ifndef _SDL2_GSlib_IMAGE_HPP_
#define _SDL2_GSlib_IMAGE_HPP_
//version modifiée le 14/07/23 (à moins d'oubli de modifier cette ligne).

	#include "SDL2_GSlib.hpp"
	
#define DEFAULT_FORMAT_USED_WITH_SMIMAGE SDL_PIXELFORMAT_RGBA8888

typedef unsigned int uint;

namespace sm /*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*/
{

#define IMGTXT_FILE_FORMAT_STR "imgtxt"
#define CURRENT_IMGTXT_VERSION 1

struct ImgTxt //support to work with pixels in a texte format
{
	/*
	Cette méthode de stockage d'image n'est pas préconisé, encore moins avec de grandes images.
	En effet, cela prend énormément plus de place que les formats d'image classiques. 
	Là où ça peut potentiellement être intéressant c'est avec des petits sprites (e.g 32*32), et 
	que l'on voudrait écrire leurs pixels directement dans le code source. 
	A part ça, à moins d'avoir une bonne raison, privilégiez les png, jpeg, bmp etc.
	*/

	std::string m_rgb_or_rgba = "RGBA";
	int m_version = CURRENT_IMGTXT_VERSION;
	bool m_is_correctly_loaded = false;
	uint32_t m_w=0;
	uint32_t m_h=0;

	std::vector<SDL_Color>  m_pixels;

	ImgTxt()=default;
	~ImgTxt()=default;

	SDL_Color getPixel(uint32_t x, uint32_t y);
	static bool saveImgTxt(ImgTxt& imgtxt, const char* file_name, bool with_comma);
	static ImgTxt loadImgTxt(const char* path);
	static ImgTxt loadImgTxt(SDL_Surface* surface);
	static SDL_Texture* imgTxtToTexture(ImgTxt& imgtxt, SDL_Renderer* ren);

};






class Image
{
	public :
		Image()= default;
		~Image();


		static SDL_Surface* textureToSurface(SDL_Texture* tex); 
		

		static bool save(SDL_Surface* img, const char* format, const char* path, short quality=70);
		

		// static SDL_Texture *changeTextureAccess(SDL_Renderer *ren, SDL_Texture *tex, int newAccess, bool destroy = true);



		void setDest(SDL_Rect dest);


		void setDestW(unsigned int val);
		

		void setDestH(unsigned int val);


		void setDestWH(unsigned int _w, unsigned int _h); 


		void setDestX(int val);
		

		void setDestY(int val);


		void setDestXY(int _x, int _y); 
		

		SDL_Rect* getDest();		
		

		int getW();
		

		int getH();


		bool isInit();


		void setDestroySDLPtrsInTheDestructor(bool new_state);


		bool getDestroySDLPtrsInTheDestructor();


		virtual bool show(bool inTheCurrentTarget = true, SDL_Rect* srcRect = nullptr)=0;
		

		virtual uint32_t getPixel(unsigned int x, unsigned int y)=0;
		

		virtual SDL_Color getPixelRGBA(unsigned int x, unsigned int y)=0;
		

		virtual SDL_Texture* getTexture()=0;
		

		virtual SDL_Surface* toSurface()=0;
		

		virtual bool save(const char* format, const char* path, short quality=70)=0;




	protected :

		SDL_Texture*		texture=nullptr;
		SDL_PixelFormat*	RGBA_fmt=nullptr;

		int 				w, h;
		int 				pitch;
		uint32_t*			pixels=nullptr;

		bool 				isInit_;
		bool 				isRen;
		SDL_Renderer*		refRen=nullptr;
		SDL_Rect 			myDestRect;
		bool 				m_destroy_SDL_ptrs_in_destructor=false;

};


















/*________________________________________________________________*/
/*________________________________________________________________*/

class ImageR : public Image 	//Image Render
{
	public :

		/*ImageR__________________BEGIN_CODE__________________________ImageR*/
		
		ImageR();
		
		ImageR(SDL_Renderer* ren, unsigned int _w = 255, unsigned int _h = 255);


		ImageR(SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color _fillColor);


		ImageR(SDL_Renderer* ren, const char* path, SDL_Color _fillColor={0,0,0,255});

		
		~ImageR();

	
		bool init(SDL_Renderer* ren, unsigned int _w = 255, unsigned int _h = 255);


		bool init(SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color _fillColor);


		bool init(SDL_Renderer* ren, const char* path, SDL_Color _fillColor={0,0,0,255});


		bool setMeAsRenderTarget();


		void restaureTheTargetOfWhenIBecameTheTarget();

		

		/*ImageR_____________________DRAW____________________________ImageR*/

		bool copy(SDL_Texture* tex, SDL_Rect* src, SDL_Rect* dst);


		bool copy(const char* path, SDL_Rect* src, SDL_Rect* dst);


		bool drawTile(SDL_Texture* tileset, int x, int y, int destB, int srcX, int srcY, int srcB);


		bool copyRect(SDL_Rect rect, SDL_Color color, short thickness = 0);


		bool copyRect(SDL_Rect rect, uint32_t  color, short thickness = 0); 


		bool fill(SDL_Color color);



		/*ImageR____________________GETERS____________________________ImageR*/

		bool show(bool inTheCurrentTarget = true, SDL_Rect* srcRect = nullptr) override;


		uint32_t getPixel(unsigned int x, unsigned int y)override;


		SDL_Color getPixelRGBA(unsigned int x, unsigned int y)override;


		SDL_Texture* getTexture() override;


		uint32_t* getAllPixels();	


		SDL_Surface* toSurface() override;


		bool save(const char* format,  const char* path, short quality=70) override;


		


	private :

		bool pixelsArrayLoaded;
		bool isModified;
		bool notYetRead;
		void defaultSettings();
		SDL_Texture *targetBeforeMe{nullptr};
		bool iHaveATargetBeforeMe{false};
		
};




















/*________________________________________________________________*/
/*________________________________________________________________*/

class ImagePPP : public Image  	//Image Pixel Per Pixel
{
	public : 

		/*ImagePPP__________________BEGIN_CODE__________________________ImagePPP*/
		
		ImagePPP(std::string _type = "RGB");


		ImagePPP(std::string _type, SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color* _fillColor = nullptr);
		

		ImagePPP(std::string _type, SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color _fillColor);


		ImagePPP(std::string _type, SDL_Renderer* ren, unsigned int _w, unsigned int _h, const char* path, SDL_Rect src={0,0,0,0}, SDL_Rect _dest={0,0,0,0});

		
		ImagePPP(std::string _type, SDL_Renderer* ren, unsigned int _w, unsigned int _h, const char* path, SDL_Rect* src, SDL_Rect _dest={0,0,0,0}); 


		ImagePPP(std::string _type, SDL_Renderer* ren, const char* path, SDL_Color _fillColor={255,255,255,255});


		~ImagePPP();


		bool init(SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color* _fillColor = nullptr);
		

		bool init(SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color _fillColor);


		bool init(SDL_Renderer* ren, unsigned int _w, unsigned int _h, const char* path, SDL_Rect src={0,0,0,0}, SDL_Rect _dest={0,0,0,0});

		
		bool init(SDL_Renderer* ren, unsigned int _w, unsigned int _h, const char* path, SDL_Rect* src, SDL_Rect _dest={0,0,0,0}); 


		bool init(SDL_Renderer* ren, const char* path, SDL_Color _fillColor={255,255,255,255});





		/*ImagePPP_____________________DRAW____________________________ImagePPP*/

		void setDrawColor(SDL_Color _draw_color);

		
		bool putPixel(unsigned int x, unsigned int y, uint32_t _color);
		

		bool putPixel(unsigned int x, unsigned int y, SDL_Color _color);
		

		bool putPixel(unsigned int x, unsigned int y);


		bool copyRect(SDL_Rect rect, uint32_t color);
		

		bool copyRect(SDL_Rect rect, SDL_Color color);


		bool fill(SDL_Color color);


		bool copyPixels(ImageR &imgRsrc, SDL_Rect src={0,0,0,0}, SDL_Rect dest={0,0,0,0}); 


		bool copyPixels(const char* path, SDL_Rect src={0,0,0,0}, SDL_Rect dest={0,0,0,0});


		bool replacePixels(uint32_t target_color, uint32_t new_color, SDL_Rect area);


		bool replacePixels(SDL_Color target_color, SDL_Color new_color, SDL_Rect area);


		bool replacePixels(uint32_t target_color, uint32_t new_color);


		bool replacePixels(SDL_Color target_color, SDL_Color new_color);



		/*ImagePPP____________________GETERS____________________________ImagePPP*/


		bool show(bool inTheCurrentTarget = true, SDL_Rect* srcRect = nullptr) override;
		

		uint32_t getPixel(unsigned int x, unsigned int y) override;
		

		SDL_Color getPixelRGBA(unsigned int x, unsigned int y) override;
		

		SDL_Texture* getTexture() override;
		

		SDL_Surface* toSurface() override;
		

		bool save(const char* format, const char* path, short quality=70)  override;






	private : 

		uint32_t superimpose(uint32_t uc1, uint32_t uc2); 
		

		std::string type;
		std::vector<const char *> typeList;

		bool 				isLock=false;
		SDL_Color 			draw_color{20, 250, 50, 255};

};














bool INTERNAL_USE_isVisibleRect(SDL_Rect rec, SDL_Rect master);
bool INTERNAL_USE_isCompletlyVisibleRect(SDL_Rect rec, SDL_Rect master);
bool INTERNAL_USE_isInvalidSrcRect(SDL_Rect& rectsrc);
bool INTERNAL_USE_isInvalidDestRect(SDL_Rect& destrect);
SDL_Rect INTERNAL_USE_cutRectForToBeAbleToDrawIt(SDL_Rect rec, SDL_Rect master); //donne l'intersection de deux rectangles
bool INTERNAL_USE_isInTheGrid(int x, int y, SDL_Rect grid);
// bool INTERNAL_USE_isSDL_PIXELFORMAT_RGBA8888(SDL_PixelFormat* fmt);
// bool INTERNAL_USE_isSDL_PIXELFORMAT_RGB888(SDL_PixelFormat* fmt);


};/*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*/




/* ce qu il reeste a faire : rajouter les constructeurs homologues des init dans ImagePPP
						   : voir si peut rajouter l ignorance d'une couleur dans copyPixels() 
						   : rajouter une fonction qui change le draw color du render pour sm::Game
						   : rajouter une fonction showIn(SDL_Texture*) pour rendercopy dans une texture en particulier

						  
*/
#endif