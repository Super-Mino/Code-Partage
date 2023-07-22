#include "SDL2_GSlib-Image.hpp"
#include <fstream>


namespace sm
{

// std::vector<char> imgr_texture_user_data{'I','t',' ','i','s',' ','a','n',' ','`','I','m','a','g','e','R','\'',' ','t','e','x','t','u','r','e','\0'};
// std::vector<char> imgppp_texture_user_data{'I','t',' ','i','s',' ','a','n',' ','`','I','m','a','g','e','P','P','P','\'',' ','t','e','x','t','u','r','e','\0'};
char* imgr_texture_user_data = (char*)"It is an `ImageR' texture";
char* imgppp_texture_user_data = (char*)"It is an `ImagePPP' texture";


SDL_Color ImgTxt::getPixel(uint32_t x, uint32_t y)
{
	// if(x >= m_w or y >= m_h) return {0,0,0,0};
	// return m_pixels.at(y*m_w +x);
	SDL_Color empty_color = {0,0,0,0};
	if(not m_is_correctly_loaded)
		return empty_color;
	return (x >= m_w or y >= m_h ? empty_color : m_pixels.at(y*m_w +x));
}



bool ImgTxt::saveImgTxt(ImgTxt& imgtxt, const char* file_path, bool with_comma)
{
	if(not imgtxt.m_is_correctly_loaded)
	{
		sm::lastError(sm::str("||Warnig|| in 'ImgTxt::saveImgTxt()', with the output file path '")+sm::str(file_path)+ sm::str("', cannot save : the given `ImgTxt' seems not loaded correctly. Try to load it with the available functions 'ImgTxt::loadImgTxt()'. If you want to load it by yourself, make sure you know what you do. "));
		return false;
	}

	std::ofstream file(file_path, std::ios::out);

	if(not file.is_open())
	{
		sm::lastError(sm::str("||Warnig|| in 'ImgTxt::saveImgTxt()', cannot open the file '")+sm::str(file_path)+ sm::str("'. "));
		return false;
	}

	std::string mode;

	if(imgtxt.m_rgb_or_rgba == "RGBA")
		mode = "RGBA";
	else
		mode = "RGB";

	/*
	La structure du fichier :
	
	imgtxt 1 (Indique le format du fichier et la version dans laquelle il est enrégistré puisqu'à l'avenir il se peut que l'organisation des fichiers 'imgtxt' évolue.)
	mode ( RGB ou RGBA )
	largeur hauteur (en nombre)
	y ('y' ou 'n' en fonction qu'il y'a les virgules ou non : 'y'[yes] il y'a, 'n'[no] il n'y'a pas)
	
	r , g , b , a , r , g , b , a , ...
	r , g , b , a , r , g , b , a , ...
	r , g , b , a , r , g , b , a , ...
	.
	.
	.

	*/

	switch(imgtxt.m_version)	//Histoire de garder la rétrocompatibilité
	{
		case CURRENT_IMGTXT_VERSION:
			{
				file << IMGTXT_FILE_FORMAT_STR << " " << (int)imgtxt.m_version << '\n';
				file << mode << '\n';
				file << (int)imgtxt.m_w << ' ' << (int)imgtxt.m_h << '\n';

				std::string separator = " ";
				if(with_comma)
				{
					separator = " , ";
					file << "y\n";
				}
				else
					file << "n\n";

				for(int i=0; i< imgtxt.m_h; i++ )
				{
					file << '\n';

					for(int j=0; j< imgtxt.m_w; j++)
					{
						SDL_Color curr_col = imgtxt.getPixel(j, i);

						std::string last_space = separator;
						if(i == imgtxt.m_h-1 and j == imgtxt.m_w-1) // = arrivé au dernier pixel
							last_space = "";

						file << (int)curr_col.r << separator;
						file << (int)curr_col.g << separator;

						if(mode == "RGBA")
						{
							file << (int)curr_col.b << separator;
							file << (int)curr_col.a << last_space;
						}	
						else
							file << (int)curr_col.b << last_space;
					}
				}
			}
			break;

		/*Si d'autres versions surviennent, d'autres 'case' seront ajouté si nécessaire.*/

		default:
			sm::lastError(sm::str("||Warnig|| in 'ImgTxt::saveImgTxt()', with the output file path '")+sm::str(file_path)+ sm::str("', the given `ImgTxt' has an invalid version. [his version : ")+std::to_string(imgtxt.m_version)+sm::str("] "));
			return false;
		
	}

	return true;
}












ImgTxt ImgTxt::loadImgTxt(const char* path)
{
	std::ifstream file(path, std::ios::in);
	
	ImgTxt imgtxt;
	
	if(not file.is_open())
	{
		sm::lastError(sm::str("||Warnig|| in 'ImgTxt::loadImgTxt()', cannot open the file '")+sm::str(path)+ sm::str("'. "));
		return imgtxt;
	}

	std::string file_format;

	file >> file_format;

	if(file_format != IMGTXT_FILE_FORMAT_STR)
	{
		sm::lastError(sm::str("||Warnig|| in 'ImgTxt::loadImgTxt()', the file '")+sm::str(path)+ sm::str("' not seems to be an ImgTxt file. "));
		return imgtxt;
	}

	int version=0;
	std::string str_version;

	try
	{
		file >> str_version;

		version = std::stoi(str_version);
	}
	catch(std::invalid_argument& er)
	{
		sm::lastError(sm::str("||Warnig|| in 'ImgTxt::loadImgTxt()', with the file '")+sm::str(path)+ sm::str("', the token of the version is invalid. "));
		return imgtxt;
	}
	catch(std::out_of_range& er)
	{
		sm::lastError(sm::str("||Warnig|| in 'ImgTxt::loadImgTxt()', with the file '")+sm::str(path)+ sm::str("', the version seems invalid. [found version : ")+str_version+sm::str("] "));
		return imgtxt;
	}

	

	switch(version)	//Histoire de garder la rétrocompatibilité
	{
		case CURRENT_IMGTXT_VERSION:
			try
			{
				imgtxt.m_version = version;
				
				std::string mode;
				file >> mode;
				if(mode == "RGBA")
					imgtxt.m_rgb_or_rgba = "RGBA";
				else
					imgtxt.m_rgb_or_rgba = "RGB";


				file >> imgtxt.m_w;
				file >> imgtxt.m_h;  

				imgtxt.m_pixels.clear();
				imgtxt.m_pixels.shrink_to_fit();

				bool with_comma;
				std::string yes_or_no;
				file >> yes_or_no;

				if(yes_or_no == "y")
					with_comma = true;
				else
				if(yes_or_no == "n")
					with_comma = false;
				else
				{
					sm::lastError(sm::str("||Warnig|| in 'ImgTxt::loadImgTxt()', with the file '")+sm::str(path)+ sm::str("', cannot identify if there are commas or not. Try to have a valid `ImgTxt' file. "));
					return ImgTxt();
				}

				for(int i=0; i< imgtxt.m_h; i++ )
				{

					for(int j=0; j< imgtxt.m_w; j++)
					{
						if(file.eof())
						{
							sm::lastError(sm::str("||Warnig|| in 'ImgTxt::loadImgTxt()', with the file '")+sm::str(path)+ sm::str("', end of file reached before finish to read requested datas. Try to have a valid `ImgTxt' file. "));
							return ImgTxt();
						}

						SDL_Color curr_col;
						std::string skip_comma;

						{
							int buf;
							file >> buf;
							curr_col.r = buf;
				
							if(with_comma)
								file >> skip_comma;
						}
														


						{
							int buf;
							file >> buf;
							curr_col.g = buf;

							if(with_comma)
								file >> skip_comma;
						}

														

						{
							int buf;
							file >> buf;
							curr_col.b = buf;

							if(with_comma)
								file >> skip_comma;
						}
														

						if(mode == "RGBA")
						{
							int buf;
							file >> buf;
							curr_col.a = buf;

							if(with_comma)
								file >> skip_comma;
						}
						else
							curr_col.a = 255;

						imgtxt.m_pixels.push_back(curr_col);
								
					}
				}
			}
			catch(std::exception& er)
			{
				sm::lastError(sm::str("||Warnig|| in 'ImgTxt::loadImgTxt()', with the file '")+sm::str(path)+ sm::str("', something went wrong when loading datas. Try to have a valid `ImgTxt' file. The exception : ")+er.what()+sm::str(" "));
				return ImgTxt();
			}

			break;

		/*Si d'autres versions surviennent, d'autres 'case' seront ajouté si nécessaire.*/

		default:
			sm::lastError(sm::str("||Warnig|| in 'ImgTxt::loadImgTxt()', with the file '")+sm::str(path)+ sm::str("', the given ImgTxt file has an invalid version. [his version : ")+std::to_string(version)+sm::str("] "));
			return ImgTxt();
	}

	imgtxt.m_is_correctly_loaded = true;

	return imgtxt;
}











ImgTxt ImgTxt::loadImgTxt(SDL_Surface* surface)
{
	ImgTxt imgtxt;

	if(not surface)
	{
		sm::lastError(sm::str("||Warnig|| in 'ImgTxt::loadImgTxt() [with a SDL_Surface*]', given surface is nullptr. "));
		return imgtxt;
	}
	

	if(surface->format->format == SDL_PIXELFORMAT_RGBA8888)
		imgtxt.m_rgb_or_rgba = "RGBA";
	else
	if(surface->format->format == SDL_PIXELFORMAT_RGB888)
		imgtxt.m_rgb_or_rgba = "RGB";
	else
	{
		sm::lastError(sm::str("||Warnig|| in 'ImgTxt::loadImgTxt() [with a SDL_Surface*]', given surface is not on RGBA8888 or RGB888 format. Sorry but for now I just work with these pixel formats. "));
		return imgtxt;
	}

	imgtxt.m_version = CURRENT_IMGTXT_VERSION;
	imgtxt.m_w = surface->w;
	imgtxt.m_h = surface->h;

	imgtxt.m_pixels.clear();
	imgtxt.m_pixels.shrink_to_fit();

	SDL_LockSurface(surface);

	for(int i=0; i< imgtxt.m_h; i++ )
	{

		for(int j=0; j< imgtxt.m_w; j++)
		{
			SDL_Color curr_col;

			uint32_t upixel = ((uint32_t*)surface->pixels)[i*imgtxt.m_w +j]; 

			SDL_GetRGBA(upixel, surface->format, 
				&curr_col.r, &curr_col.g, &curr_col.b, &curr_col.a);

			if(imgtxt.m_rgb_or_rgba == "RGB")
				curr_col.a = 255;

			imgtxt.m_pixels.push_back(curr_col);
		}
	}

	SDL_UnlockSurface(surface);
	imgtxt.m_is_correctly_loaded = true;

	return imgtxt;
}










SDL_Texture* ImgTxt::imgTxtToTexture(ImgTxt& imgtxt, SDL_Renderer* ren)
{
	if(not imgtxt.m_is_correctly_loaded)
		return nullptr;

	ImagePPP imgppp;
	imgppp.init(ren, imgtxt.m_w, imgtxt.m_h);

	try
	{
		for(int i = 0; i< imgtxt.m_h; i++)
		{
			for(int j = 0; j< imgtxt.m_w; j++)
			{
				imgppp.putPixel(j, i, imgtxt.getPixel(j, i));
				
			}
		}

	}
	catch(std::exception& er)
	{
		return nullptr;
	}

	// for(int i = 0; i< imgtxt.m_h; i++)
	// {
	// 	for(int j = 0; j< imgtxt.m_w; j++)
	// 	{
	// 		imgppp.putPixel(j, i, imgtxt.getPixel(j, i));
			
	// 	}
	// }
	
	return imgppp.getTexture();
}













/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/



Image::~Image()
{
	if(m_destroy_SDL_ptrs_in_destructor)
	{
		if(texture)
			SDL_DestroyTexture(texture);

		if(RGBA_fmt)
			SDL_FreeFormat(RGBA_fmt);
	}
	else
	{
		sm::addToClearAtTheEnd(texture);
		sm::addToClearAtTheEnd(RGBA_fmt);
	}
}


SDL_Surface *Image::textureToSurface(SDL_Texture *tex)
{
	/*Cette fonction ne marche que pour les textures en accès streaming, donc bien adapté pour les ImagePPP.*/

	SDL_Surface *su;
	uint32_t *texPix = nullptr;
	int pitch;

	uint32_t format;
	int _w, _h;

	

	if(SDL_LockTexture(tex, nullptr, (void**) &texPix, &pitch) != 0)
		logMessage("Error : in fonction `Image::TextureToSurface()' error at `SDL_LockTexture'", SDL_GetError());
	else
		if(SDL_QueryTexture(tex, &format, nullptr, &_w, &_h) != 0)
			logMessage("Error : in function `Image::TextureToSurface()' error at `SDL_QueryTexture'", SDL_GetError());
		else
		{
			su = SDL_CreateRGBSurfaceWithFormatFrom(texPix, _w, _h, 32, /*_w *sizeof(uint32_t)*/pitch, format);
			SDL_UnlockTexture(tex);
			return su;
		}


	SDL_UnlockTexture(tex);
	return nullptr;

}

/*===================================================================================
|																					|
====================================================================================*/


bool Image::save(SDL_Surface* img, const char* format, const char* path, short quality)
{
	bool success = false;


	if(std::string(format)   ==   std::string("BMP") 
		|| std::string(format) == std::string("bmp"))
	{
		int res = SDL_SaveBMP(img, path);
		if (res == 0)
			success = true;
		else
			lastError(std::string("Warnig, in Image::save() : ") 
						+ std::string(SDL_GetError())); 
	}


	else
	if(std::string(format)   ==   std::string("PNG") 
		|| std::string(format) == std::string("png"))
	{
		int res = IMG_SavePNG(img, path);
		if (res == 0)
			success = true;
		else
			lastError(std::string("Warnig, in Image::save() : ") 
						+ std::string(SDL_GetError())); 
	}


	else
	if(std::string(format)   ==   std::string("JPG") 
		|| std::string(format) == std::string("jpg")
		|| std::string(format) == std::string("jpeg")
		|| std::string(format) == std::string("JPEG"))
	{
		int res = IMG_SaveJPG(img, path, quality);
		if (res == 0)
			success = true;
		else
			lastError(std::string("Warnig, in Image::save() : ") 
						+ std::string(SDL_GetError())); 
	}


	return success;
}

/*===================================================================================
|																					|
====================================================================================*/

SDL_Rect* Image::getDest()	{return &myDestRect;}

/*===================================================================================
|																					|
====================================================================================*/

void Image::setDest(SDL_Rect dest) {myDestRect = dest;}

/*===================================================================================
|																					|
====================================================================================*/

void Image::setDestW(unsigned int val) {myDestRect.w = val;}
		
/*===================================================================================
|																					|
====================================================================================*/

void Image::setDestH(unsigned int val) {myDestRect.h = val;}

/*===================================================================================
|																					|
====================================================================================*/

void Image::setDestWH(unsigned int _w, unsigned int _h) {myDestRect.w = _w; myDestRect.h = _h;}

/*===================================================================================
|																					|
====================================================================================*/

void Image::setDestX(int val) {myDestRect.x = val;}
		
/*===================================================================================
|																					|
====================================================================================*/

void Image::setDestY(int val) {myDestRect.y = val;}

/*===================================================================================
|																					|
====================================================================================*/

void Image::setDestXY(int _x, int _y) {myDestRect.x = _x; myDestRect.y = _y;}

/*===================================================================================
|																					|
====================================================================================*/

int Image::getW()  {return w;}

/*===================================================================================
|																					|
====================================================================================*/

int Image::getH()  {return h;}

/*===================================================================================
|																					|
====================================================================================*/

bool Image::isInit() {return isInit_;}

/*===================================================================================
|																					|
====================================================================================*/

void Image::setDestroySDLPtrsInTheDestructor(bool new_state)
{
	m_destroy_SDL_ptrs_in_destructor = new_state;
}

/*===================================================================================
|																					|
====================================================================================*/

bool Image::getDestroySDLPtrsInTheDestructor()
{
	return m_destroy_SDL_ptrs_in_destructor;
}




/*===================================================================================
|																					|
====================================================================================*/

// SDL_Texture *Image::changeTextureAccess(SDL_Renderer *ren, SDL_Texture *tex, int newAccess, bool destroy)
	// {
	// 	SDL_Texture *newTex = nullptr;
	// 	uint32_t format;
	// 	int access, w, h;


	// 	SDL_QueryTexture(tex, &format, &access, &w, &h);


	// 	if(access != newAccess)
	// 	{
	// 		newTex = SDL_CreateTexture(ren, format, newAccess, w, h);
	// 		if(newTex == nullptr)
	// 			logMessage("Error : in funtion `changeTextureAccess' : ", SDL_GetError());
	// 		else
	// 		{

	// 		}
	// 	}
	// 	else 
	// 		return tex;
	// }




/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/



							void ImageR::defaultSettings()
							{
								isInit_ = false;
								isModified = false;
								pixelsArrayLoaded = false;
								isRen = false;
								notYetRead = true;
							}

							ImageR::ImageR() 
							{
								defaultSettings();
							}

				ImageR::ImageR(SDL_Renderer* ren, unsigned int _w, unsigned int _h)
				{
					defaultSettings();
					init(ren, _w, _h);
				}


		ImageR::ImageR(SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color _fillColor)
		{
			defaultSettings();
			init(ren, _w, _h, _fillColor);
		}


				ImageR::ImageR(SDL_Renderer* ren, const char* path, SDL_Color _fillColor)
				{
					defaultSettings();
					init(ren, path, _fillColor);
				}


							ImageR::~ImageR()
							{
							// 	if(texture != nullptr)
							// 		SDL_DestroyTexture(texture);
							// 	if(RGBA_fmt != nullptr)
							// 		SDL_FreeFormat(RGBA_fmt);

								if(pixelsArrayLoaded)
								{
									delete[] pixels;
									pixelsArrayLoaded = false;
								}

							}

/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::init(SDL_Renderer *ren, unsigned int _w, unsigned int _h)
{
	if(isInit_)
	{
		lastError("||Warning|| in 'ImageR::init()' : cannot init if is already init");
		return false; 
	}


	RGBA_fmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);					//Init le format
	
	if(not RGBA_fmt)
	{
		sm::lastError(sm::str("||Error|| in `ImageR::init()' : loading pixel format failled : ")+sm::str(SDL_GetError())+sm::str(" "));
		return false;
	}

	sm::addToClearAtTheEnd(RGBA_fmt);


	w = _w; 
	h = _h;
	myDestRect.x = 0; myDestRect.y = 0;										//Init le rectangle de dessin sur rendu. Par défaut ça a la même taille que la texture
	myDestRect.w = w; myDestRect.h = h;


	if(ren != nullptr)														//Init la référence au renderer
	{
		refRen = ren;
		isRen = true;
	}
	else
	{
		refRen = nullptr;
		isRen = false;
		lastError("||Warning|| in `ImageR::init()' : renderer given is nullptr or invalid");
	}



	if(isRen)																//Init la texture en target, avec le format RGBA 32 bits
	{
		texture = SDL_CreateTexture(refRen, SDL_PIXELFORMAT_RGBA8888, 
									SDL_TEXTUREACCESS_TARGET, w, h);
		if(texture == nullptr)
		{
			isInit_ = false;
			lastError(str("||Warning|| in `ImageR::init()' : create texture not done") + str(SDL_GetError()));
		}
		else
		{
			isInit_ = true;
			
			SDL_SetTextureUserData(texture, (void*)imgr_texture_user_data);
			sm::addToClearAtTheEnd(texture);
		}

	}
	else
		isInit_ = false;


	if(!isInit_)
		SDL_FreeFormat(RGBA_fmt);

	return isInit_;
}


/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::init(SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color _fillColor)
{
	if(isInit_)
	{
		lastError("||Warning|| in 'ImageR::init()' : cannot init if is already init");
		return false; 
	}

	bool success1, success2;
	success1 = init(ren, _w, _h);

	if(success1)
	{
		SDL_Rect allTexture = {0, 0, w, h};
		success2 = copyRect(allTexture, _fillColor); 
	}
	else
		return false;

	return success1 == true and success2 == true;
}


/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::init(SDL_Renderer *ren, const char* path, SDL_Color _fillColor)
{
	if(isInit_)
	{
		lastError("||Warning|| in 'ImageR::init()' : cannot init if is already init");
		return false; 
	}


	SDL_Texture* tmp = sm::loadImg(ren, path);
	if(tmp == nullptr)
	{
		lastError(std::string("||Warnig|| cannot init ImageR : ") + std::string(SDL_GetError()));
		return false;
	}


	int width = 0, height = 0;
	SDL_QueryTexture(tmp, nullptr, nullptr, &width, &height);

	if(!init(ren, width, height, _fillColor))
	{
		SDL_DestroyTexture(tmp);
		return false;
	}


	if(!copy(tmp, nullptr, nullptr))
	{
		SDL_DestroyTexture(tmp);
		if(texture != nullptr)
			SDL_DestroyTexture(texture);
		w = 0; h = 0;

		if(RGBA_fmt != nullptr)
			SDL_FreeFormat(RGBA_fmt);

		isInit_ = false;
		return false;
	}


	SDL_DestroyTexture(tmp);
	isInit_ = true;
	return true;

}


/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::setMeAsRenderTarget()
{
	if(isInit_)
	{
		short res;

		targetBeforeMe = SDL_GetRenderTarget(refRen);
		res = SDL_SetRenderTarget(refRen, texture);

		if(res != 0)
		{
			lastError("||Warnig|| in ImageR::setMeAsRenderTarget() : " 
						+ str(SDL_GetError()));
			return false;
		}

		iHaveATargetBeforeMe = true;
		return true;
	}
	else 
		lastError("||Warnig|| in ImageR::setMeAsRenderTarget() : cannot be target if not init");
	
	return false;
}

/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

void ImageR::restaureTheTargetOfWhenIBecameTheTarget()
{
	if(isInit_)
	{
		if(iHaveATargetBeforeMe)
		{
			SDL_SetRenderTarget(refRen, targetBeforeMe);
			iHaveATargetBeforeMe = false;
		}
		else
			lastError("||Warnig|| in ImageR::restaureTheTargetOfWhenIBecameTheTarget() : no target can be restaured for this instance of ImageR yet");
	}
	else 
		lastError("||Warnig|| in ImageR::restaureTheTargetOfWhenIBecameTheTarget() : no target can be restaured for this instance of ImageR yet");
}

/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::copy(SDL_Texture* tex, SDL_Rect* src, SDL_Rect* dst)
{
	// if(dst != nullptr)
		// if(dst->x > w || dst->y > h)
			// return false;

	bool success = true;

	if(isInit_)
	{
		SDL_Texture* previousTarget = SDL_GetRenderTarget(refRen);

		SDL_SetRenderTarget(refRen, texture);

		int res = SDL_RenderCopy(refRen, tex, src, dst);
		if(res != 0)
		{
			lastError("||Warnig|| in ImageR::copy() : " 
						+ str(SDL_GetError()));
			success = false;
		}

		SDL_SetRenderTarget(refRen, previousTarget);

		isModified = true;

		return success;
	
	}
	lastError(std::string("||Warnig|| ImageR not init. "));
	return false;
}

/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::copy(const char* path, SDL_Rect* src, SDL_Rect* dst)
{
	if(isInit_)
	{
		SDL_Texture* tmp = loadImg(refRen, path);
		if(tmp == nullptr)
			return false;
		
		copy(tmp, src, dst);
		SDL_DestroyTexture(tmp);
		return true;
	}
	lastError(std::string("||Warnig|| ImageR not init : "));
	return false;
}

/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::drawTile(SDL_Texture *tileset, int x, int y, int destB, int srcX, int srcY, int srcB)
{
	return sm::drawTile(refRen, texture, tileset, x, y, destB, srcX, srcY, srcB, true);
}
/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::copyRect(SDL_Rect rect, SDL_Color color, short thickness)
{
	SDL_Color before_render_color;								//Prise en compte des propiétés de rendu avant opération
	SDL_Texture* previousTarget = SDL_GetRenderTarget(refRen);
	SDL_GetRenderDrawColor(refRen, &before_render_color.r,
								   &before_render_color.g,
								   &before_render_color.b,
								   &before_render_color.a);




	bool _fill = false, withThickness = false,					//On dessine notre rectangle et co
		 externThickness = false, success = true;


	if(thickness == 0)
	{
		_fill = true; 					//ici fill c'est pour remplir ou non le rectangle à dessiner, et non remplir toute la portée 
	}
	else if(thickness < 0 )
	{
		externThickness = true;
	}
	else
		withThickness = true;

	if(withThickness)
	{
		int half;  // ==> moitié

		if(rect.w <= rect.h)			//On regarde quel côté a la plus petite dimension pour savoir si l'épaiseur ne couvrira pas déjà tout le rectangle.
			half = rect.w/2;
		else
			half = rect.h/2;

		if(thickness >= half)			//Si l'épaisseur est égale ou supérieur à la moitié de la plus petite dimension, alors tout le rect sera rampli.
		{
			withThickness = false;
			_fill = true;
		}
		
	}



	/******************************************/

	int erreur;

	using std::operator ""s;

	erreur = SDL_SetRenderDrawColor(refRen, color.r, color.g,			
								   color.b, color.a);
		if(erreur != 0) {
			lastError("||Warnig|| in ImageR::copyRect() : "s + str(SDL_GetError()) );
			success = false; goto err; }

	erreur =  SDL_SetRenderTarget(refRen, texture); 
		if(erreur != 0) {
			lastError("||Warnig|| in ImageR::copyRect() : "s + str(SDL_GetError()) );
			success = false; goto err; }





	if(_fill)
	{
		erreur = SDL_RenderFillRect(refRen, &rect);
			if(erreur != 0) {
				lastError("||Warnig|| in ImageR::copyRect() : "s + str(SDL_GetError()) );
				success = false; goto err; }
	}
	else 



	if(withThickness)		//Le dessin de plusieurs rectangles emboîtés permettrons de donner une épaisseur au retangle final
	{												//Pour rappel : SDL_RenderDrawRect() dessine avec une épaisseur de 1 pixel
		int count = 0;
		while(count < thickness)
		{
			erreur = SDL_RenderDrawRect(refRen, &rect);
				if(erreur != 0) {
					lastError("||Warnig|| in ImageR::copyRect() : "s + str(SDL_GetError()) );
					success = false; goto err; }


			rect.w-= 2;		//Ces quatres lignes représentent une réduction centrée du rect
			rect.h-= 2;
			rect.x++;
			rect.y++;

			++count;
		}	
	}
	else



	if(externThickness)
	{
		int count = 0;
		while(count > thickness)
		{
			erreur = SDL_RenderDrawRect(refRen, &rect);
				if(erreur != 0) {
					lastError("||Warnig|| in ImageR::copyRect() : "s + str(SDL_GetError()) );
					success = false; goto err; }

			rect.w+= 2;		//Ces quatres lignes représentent un angrandissement centré du rect
			rect.h+= 2;
			rect.x--;
			rect.y--;

			--count;
		}	
	}
	
	/******************************************/


	err :


	SDL_SetRenderTarget(refRen, previousTarget);					//On remet enfin les propiétés telles qu'étaient avant
	SDL_SetRenderDrawColor(refRen, before_render_color.r,
								   before_render_color.g,
								   before_render_color.b,
								   before_render_color.a);
	
	return success;
		
}


/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::copyRect(SDL_Rect rect, uint32_t color, short thickness)
{
	SDL_Color c;
	SDL_GetRGBA(color, RGBA_fmt, &c.r, &c.g, &c.b, &c.a);
	return copyRect(rect, c, thickness);
}

/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::fill(SDL_Color color)
{
	return copyRect({0, 0, w, h}, color, w);
}

/************************************************************************************
*																					*
*																					*
*					------------------------------------							*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::show(bool inTheCurrentTarget, SDL_Rect* srcRect) 
{
	if(!isInit_) 
	{
		lastError(std::string("||Warnig|| in ImageR::show(), ImageR not init.    "));
		return false;
	}

	bool restoreTarget = false;
	SDL_Texture* previousTarget = SDL_GetRenderTarget(refRen);

	if(!inTheCurrentTarget)			//si non pour la cible courrante, on force alors la copie sur le render
	{
		SDL_SetRenderTarget(refRen, nullptr);
		restoreTarget = true;
	}


	SDL_RenderCopy(refRen, texture, srcRect, &myDestRect);

	if(restoreTarget)
		SDL_SetRenderTarget(refRen, previousTarget);

	return true;
}

/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

uint32_t ImageR::getPixel(unsigned int x, unsigned int y) 
{
	if( !isInit_  )
	{
		lastError(std::string("||Warnig|| in ImageR::getPixel(), ImageR not init : "));
		return 0;
	}
	else
	if(x < 0 || x > w-1   ||   y < 0 || y > h-1)
	{
		lastError(std::string("||Warnig|| in ImageR::getPixel(), given pos not in the grid : "));
		return 0;
	}
	else
	{

		if(isModified || notYetRead )
		{
			if(!pixelsArrayLoaded)
			{
				pitch = sizeof(uint32_t) * w;
				try 
				{
					pixels = new uint32_t[pitch*h];
				}
				catch(int e) 
				{
					exit(-1);
				}
				pixelsArrayLoaded = true;
			}

			SDL_Texture* previousTarget = SDL_GetRenderTarget(refRen);
			SDL_SetRenderTarget(refRen, texture);
			SDL_RenderReadPixels(refRen, nullptr, SDL_PIXELFORMAT_RGBA8888, pixels, pitch);
			SDL_SetRenderTarget(refRen, previousTarget);
			isModified = false;
			notYetRead = false;
		}
			
		uint32_t uc = pixels[y*w +x];
		

		return uc;
	}
	return 1;
	
}

/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

SDL_Color ImageR::getPixelRGBA(unsigned int x, unsigned int y)
{
	if(!isInit_) 
	{
		lastError(std::string("||Warnig|| in ImageR::getPixelRGBA(), ImageR not init : "));
		return {0, 0, 0, 0};
	}

	SDL_Color c;
	SDL_GetRGBA(getPixel(x, y), RGBA_fmt, &c.r, &c.g, &c.b, &c.a);
	return c;
}


/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

SDL_Texture* ImageR::getTexture()  {return texture;}


/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

uint32_t* ImageR::getAllPixels()
{
	if(isInit_)
	{
		if(isModified || notYetRead)
		{
			if(!pixelsArrayLoaded)
			{
				pitch = sizeof(uint32_t) * w;
				try 
				{
					pixels = new uint32_t[pitch*h];
				}
				catch(std::exception& ex) 
				{
					lastError(sm::str("||Fatal error|| : in 'ImageR::getAllPixels()' error of memory allocation : ")+ex.what());
					return nullptr;
					// exit(-1);
				}
				pixelsArrayLoaded = true;
			}

			SDL_Texture* previousTarget = SDL_GetRenderTarget(refRen);
			SDL_SetRenderTarget(refRen, texture);
			SDL_RenderReadPixels(refRen, nullptr, SDL_PIXELFORMAT_RGBA8888, pixels, pitch);
			SDL_SetRenderTarget(refRen, previousTarget);
			notYetRead = false;
			isModified = false;
		}

		return pixels;

	}
	
	lastError(std::string("||Warnig|| in ImageR::getAllPixels(), ImageR not init : "));
	return nullptr;
}	

/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

SDL_Surface* ImageR::toSurface()
{
	uint32_t* pix = getAllPixels();

	return SDL_CreateRGBSurfaceWithFormatFrom(pix, w, h, 32, w * sizeof(uint32_t), DEFAULT_FORMAT_USED_WITH_SMIMAGE);
}

/************************************************************************************
*																					*
*																					*
*																					*
*																					*
*																					*
*																					*
*************************************************************************************/

bool ImageR::save(const char* format, const char* path, short quality)
{
	SDL_Surface* tmp = toSurface();
	bool success = false;


	success = Image::save(tmp, format, path, quality);

	if (!success)
		lastError(std::string("Warnig, in ImageR::save() : ") 
					+ std::string(SDL_GetError())); 


	if(tmp != nullptr)
		SDL_FreeSurface(tmp);

	return success;
}


/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

						ImagePPP::ImagePPP(std::string _type) : Image()
						{
							typeList.push_back("RGB");
							typeList.push_back("RGBA");


							if(_type == typeList[0])
								type = typeList[0];

							else if(_type == typeList[1])
								type = typeList[1];

							else
								type = typeList[0];

								isRen = false;
								isInit_ = false;
								isLock = false;

						}

				ImagePPP::ImagePPP(std::string _type, SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color* _fillColor)
				: ImagePPP(_type)
				{
					init(ren, _w, _h, _fillColor);
				}

		ImagePPP::ImagePPP(std::string _type, SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color _fillColor)
		: ImagePPP(_type)
		{
			init(ren, _w, _h, _fillColor);
		}

ImagePPP::ImagePPP(std::string _type, SDL_Renderer* ren, unsigned int _w, unsigned int _h, const char* path, SDL_Rect src, SDL_Rect _dest)
: ImagePPP(_type)
{
	init(ren, _w, _h, path, src, _dest);
}

		ImagePPP::ImagePPP(std::string _type, SDL_Renderer* ren, unsigned int _w, unsigned int _h, const char* path, SDL_Rect* src, SDL_Rect _dest)
		: ImagePPP(_type)
		{
			init(ren, _w, _h, path, src, _dest);
		}

				ImagePPP::ImagePPP(std::string _type, SDL_Renderer* ren, const char* path, SDL_Color _fillColor)
				: ImagePPP(_type)
				{
					init(ren, path, _fillColor);
				}


						/*_________________________________________*/

							ImagePPP::~ImagePPP()
							{
								// SDL_DestroyTexture(texture);
								// SDL_FreeFormat(RGBA_fmt);
								SDL_UnlockTexture(texture);
							}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

bool ImagePPP::init(SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color *_fillColor)
{
	if(isInit_) 
	{
		lastError("||Warning|| in 'ImagePPP::init()' : cannot init if is already init");
		return false; 
	}


	RGBA_fmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);			//Init le format
	
	if(not RGBA_fmt)
	{
		sm::lastError(sm::str("||Error|| in `ImagePPP::init()' : loading pixel format failled : ")+sm::str(SDL_GetError())+sm::str(" "));
		return false;
	}

	sm::addToClearAtTheEnd(RGBA_fmt);

	w = _w; 
	h = _h;
	myDestRect.x = 0; myDestRect.y = 0;								//Init le rectangle de dessin sur rendu. Par défaut ça a la même taille que la texture
	myDestRect.w = w; myDestRect.h = h;


	if(ren != nullptr)												//Init la référence au renderer
	{
		refRen = ren;
		isRen = true;
	}
	else
	{
		refRen = nullptr;
		isRen = false;
		logMessage("Warning : in `ImagePPP::init()' : renderer given is nullptr or invalid");
	}



	if(isRen)														//Init la texture en streaming, avec le format RGBA 32 bits
	{
		texture = SDL_CreateTexture(refRen, SDL_PIXELFORMAT_RGBA8888, 
									SDL_TEXTUREACCESS_STREAMING, w, h);
		if(texture == nullptr)
		{
			isInit_ = false;
			logMessage("Warning : in `ImagePPP::init()' : create texture not done", SDL_GetError());
		}
		else
		{
			isInit_ = true;
			sm::addToClearAtTheEnd(texture);
			SDL_SetTextureUserData(texture, (void*)imgppp_texture_user_data);
		}

	}
	else
	{
		lastError("||Error|| in `ImagePPP::init()' : renderer given is nullptr or invalid");
		isInit_ = false;
	}



	if(isInit_)														//remplissage de la texture avec la couleur demandé ou non
	{
		uint8_t r = 255;
		uint8_t g = 255;
		uint8_t b = 255;
		uint8_t a = 255;

		if(_fillColor != nullptr)
		{
			r = _fillColor->r;
			g = _fillColor->g;
			b = _fillColor->b;
			if(type == typeList[1]) a = _fillColor->a; 				//type 1 ---> RGBA --> on prend en compte la transparence
		}

		uint32_t fillColor = SDL_MapRGBA(RGBA_fmt, r, g, b, a);


	
		int locked = SDL_LockTexture(texture, nullptr, (void**) &pixels, &pitch);
		if (locked != 0)
		{
			lastError(std::string("|| fail in ImagePPP::init() when trying to lock texture to fill with color : ") 
						+ std::string(SDL_GetError()));
			isInit_ = false;
		}

		else
		{
			for(int y = 0; y< h; ++y)
			{
				for(int x = 0; x< w; ++x)
				{
					pixels[y*w+x] = fillColor;
				} 
			}
			SDL_UnlockTexture(texture);
		}

		
	}
	
	if(!isInit_)
		SDL_FreeFormat(RGBA_fmt);



	return isInit_;
};


/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


bool ImagePPP::init(SDL_Renderer* ren, unsigned int _w, unsigned int _h, SDL_Color _fillColor)
{
	return init(ren, _w, _h, &_fillColor);
}


/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


bool ImagePPP::init(SDL_Renderer* ren, unsigned int _w, unsigned int _h, const char *path, SDL_Rect src, SDL_Rect _dest)
{
	if(not init(ren, _w, _h))
	{		
		lastError("||Error|| in 'ImagePPP::init()' : cannot init requierements");
		return false;
	}

	SDL_Rect dest={0, 0, (int)_w, (int)_h}; 

	if(INTERNAL_USE_isInvalidDestRect(_dest)) //si est par défaut
	{
		/*passe*/
	}
	else
	{
		dest = _dest;
	}

	// ImageR imgR;

	// if(imgR.init(ren, path) == false)
	// {
	// 	lastError("||Error|| in 'ImagePPP::init()' : cannot init imgR");
	// 	return false;
	// }

	// return copyPixels(imgR, src, dest);

	return copyPixels(path, src, dest); 
}


/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


bool ImagePPP::init(SDL_Renderer* ren, unsigned int _w, unsigned int _h, const char* path, SDL_Rect* src, SDL_Rect _dest)
{
	SDL_Rect recSrc = {0,0,0,0};
	if(src != nullptr)
		recSrc = *src;
	return init(ren, _w, _h, path, recSrc, _dest);
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

bool ImagePPP::init(SDL_Renderer* ren, const char* path, SDL_Color _fillColor)
{
	SDL_Color _blanc_ = {255, 255, 255, 255}; //afin de faire correspondre la couleur de fond par défaut de l'ImageR avec celle des ImagePPP

	ImageR imgR(ren, path, _blanc_);

	if(not imgR.isInit())
		return false; //imgR s'est déjà occupé du message d'erreur

	
	bool success = init(ren, imgR.getW(), imgR.getH(), _fillColor);  

	if(not success)
		return false; 

	success = copyPixels(imgR); 

	if(not success)
	{
		sm::lastError(sm::str("from 'ImagePPP::init()' : ")+sm::lastError());
		return false;
	}

	return success; // = true;
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


void ImagePPP::setDrawColor(SDL_Color _draw_color)
{
	draw_color = _draw_color;
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

bool ImagePPP::putPixel(unsigned int x, unsigned int y, uint32_t _color)
{
	if(isInit_)
	{
		if(x < 0 || x > w-1   ||   y < 0 || y > h-1)
			return false;   

		if(!isLock)
		{
			int locked = SDL_LockTexture(texture, nullptr, (void**) &pixels, &pitch);
			if (locked != 0)
			{
				lastError(std::string("|| fail in ImagePPP::putPixel() when trying lock texture : ") 
							+ std::string(SDL_GetError()));
				return false;
			}
			else
			isLock = true;
		}

		if(type == str("RGBA"))
			_color = superimpose(_color, pixels[y*w +x]);

		pixels[y*w +x] = _color;
		

		return true;
		
	}

	return false;
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

bool ImagePPP::putPixel(unsigned int x, unsigned int y, SDL_Color _color)
{
	if(isInit_)
	{
		uint8_t r = _color.r;
		uint8_t g = _color.g;
		uint8_t b = _color.b;
		uint8_t a = 255;
		if(type == typeList[1]) a = _color.a;

	
		uint32_t uc = SDL_MapRGBA(RGBA_fmt, r, g, b, a); // uc = uint32_t color  :)
		return putPixel(x, y, uc);

	}

	return false;
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

bool ImagePPP::putPixel(unsigned int x, unsigned int y)
{
	return putPixel(x, y, draw_color);
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

bool ImagePPP::copyRect(SDL_Rect rect, uint32_t color)
{
	if(!isInit_)
	{
		lastError(std::string("|| in ImagePPP::copyRect(), cannot do it if is not init"));
		return false;
	}


	if(!isLock)
	{
		int locked = SDL_LockTexture(texture, nullptr, (void**) &pixels, &pitch);
		if (locked != 0)
		{
			lastError(std::string("|| fail in ImagePPP::copyRect() when trying lock texture : ") 
						+ std::string(SDL_GetError()));
			return false;
		}
		else
		isLock = true;
	}


	if(!INTERNAL_USE_isVisibleRect(rect, {0, 0, static_cast<int>(w), static_cast<int>(h)}))
		return true; //on retourne false qu'en cas d'erreur, ici c'en est pas vraiment un

	if(not INTERNAL_USE_isCompletlyVisibleRect(rect, {0, 0, static_cast<int>(w), static_cast<int>(h)}))
		//tailler le rect pour n'avoir que la partie visible
		rect = INTERNAL_USE_cutRectForToBeAbleToDrawIt(rect, {0, 0, static_cast<int>(w), static_cast<int>(h)});


	int x_start = rect.x;
	int y_start = rect.y;

	for(int y = 0; y < rect.h; y++)						//parcour de chaque pixel du rect à dessiner
	{
		for(int x = 0; x < rect.w; x++)
		{
			int real_x = x + x_start;
			int real_y = y + y_start;

			if(real_x < w && real_y < h)  				//Je garde quand-même cette vérification par sûreté. 
				if(real_x >= 0 && real_y >= 0)			// = si les coo du pixel sont bien dans les limites de la porté 
				{
					uint32_t _color = color;
					if(type == str("RGBA"))
						_color = superimpose(_color, pixels[real_y*w +real_x]);
					pixels[real_y*w +real_x] = _color;
				}
		}
	}
	
	return true;
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

bool ImagePPP::copyRect(SDL_Rect rect, SDL_Color color)
{
	uint32_t uc = SDL_MapRGBA(RGBA_fmt, color.r, color.g, color.b, color.a);
	return copyRect(rect, uc);
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

bool ImagePPP::fill(SDL_Color color)
{
	return copyRect({0, 0, w, h}, color);
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

bool ImagePPP::copyPixels(ImageR &imgRsrc, SDL_Rect src, SDL_Rect dest)
{

	/*
	Cette fonction copie-colle une partie (ou la totale) d'une image source vers la partie indiqué sur cette ImagePPP.
	*/
	if(not isInit_)
	{
		lastError("||Warnig|| in ImagePPP::copyPixels(), cannot copy : not yet init");
		return false;
	}

	uint32_t *pixels_to_copy = imgRsrc.getAllPixels(); 
	
	if(pixels_to_copy == nullptr)
	{
		lastError("||Error|| in 'ImagePPP::copyPixels()' cannot get source pixels from imgRsrc");
		return false;
	}


	bool src_en_compte = true;

	if(INTERNAL_USE_isInvalidSrcRect(src))//si la source est invalid ( ou comme initialisé par défaut)
		src_en_compte = false;

	

	if(INTERNAL_USE_isInvalidDestRect(dest)) //si est par défaut, la dest devient toute la portée 
	{
		dest.x = 0; dest.y = 0; dest.w = w; dest.h = h; 
	}
	

	if(not isLock)
	{
		int locked = SDL_LockTexture(texture, nullptr, (void**) &pixels, &pitch); 
		if(locked != 0)
		{
			lastError(std::string("|| fail in ImagePPP::copyPixels() when trying lock texture : ") 
							+ std::string(SDL_GetError()));
			return false;
		}
		else
		isLock = true;
	}


	for( int y = dest.y; y < dest.y + dest.h; y++)
	{
		if(y < 0 || y > h)//vérification que la colonne qui va recevoir le pixel fait parti de la portée de this
			continue;

		for(int x = dest.x; x < dest.x + dest.w; x++)
		{
			if(x < 0 || x > w)//vérification que la ligne qui va recevoir le pixel fait aussi parti de la portée de this
				continue; 


			int x_src = src.x+x-dest.x; //on prend la source par défaut, c-à-d à partir 0;0 à la dest.w;dest.h
			int y_src = src.y+y-dest.y;

		

			if(src_en_compte) //mais on applique le décalage si on ne prend qu'une certaine partie des pixels à copier
			{
				x_src += src.x;
				y_src += src.y; 

				if(x_src >= src.x + src.w) //si la source a déjà été lue au complet on quitte
					break;
				if(y_src >= src.y + src.h)
					break;

			}


			if(x_src < 0 || x_src >= imgRsrc.getW() || y_src < 0 || y_src >= imgRsrc.getH()) //vérification finale que le pixel courant fait bien parti de la source 
				continue; 
			

			uint32_t _color = pixels_to_copy[y_src*imgRsrc.getW() + x_src];

			if(type == str("RGBA"))
				uint32_t _color = superimpose(_color, pixels[y*w +x]); 
			
			pixels[y*w +x] = _color; 

			 // pixels[y*w +x] = pixels_to_copy[y_src*imgRsrc.getW() + x_src];
		} 
	}

	return true;
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


bool ImagePPP::copyPixels(const char* path, SDL_Rect src, SDL_Rect dest)
{
	ImageR imgR;

	if(imgR.init(refRen, path) == false)
	{
		lastError("||Error|| in 'ImagePPP::copyPixels()' : cannot init imgR");
		return false;
	}

	return copyPixels(imgR, src, dest); 
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


bool ImagePPP::replacePixels(uint32_t target_color, uint32_t new_color, SDL_Rect area)
{
	if(not isInit_)
	{
		sm::lastError("||Warnig|| in ImagePPP::replacePixels(), cannot do : not yet init");
		return false;
	}

	SDL_Rect self_rect = {0, 0, w, h};

	if(not INTERNAL_USE_isVisibleRect(area, self_rect))
	{
		sm::lastError("||Warnig|| in ImagePPP::replacePixels(), cannot do : invalide area (dest rect)");
		return false;
	}


	if(not isLock)
	{
		int locked = SDL_LockTexture(texture, nullptr, (void**) &pixels, &pitch);
		if(locked != 0)
		{
			sm::lastError(std::string("|| fail in ImagePPP::replacePixels() when trying lock texture : ") 
							+ std::string(SDL_GetError()));
			return false;
		}
		else
		isLock = true;
	}


	for(int y=0; y< area.h; ++y)	//parcour de la zone à remplacer les pixels
	{
		for(int x=0; x< area.w; ++x)
		{
			int real_x = x+area.x; 	//décalage des coo dans leur vrai place
			int real_y = y+area.y; 

			if(not INTERNAL_USE_isInTheGrid(real_x, real_y, {0, 0, w, h})) //la grille de pixel 
				continue; 

			if(pixels[real_y*w +real_x] == target_color)
				pixels[real_y*w +real_x] = new_color;

		}
	}

	return true;
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


bool ImagePPP::replacePixels(SDL_Color target_color, SDL_Color new_color, SDL_Rect area)
{
	if(not isInit_) 
	{
		sm::lastError("||Warnig|| in ImagePPP::replacePixels(), cannot do : not yet init");
		return false;
	}
	

	SDL_Color& tc = target_color, nc = new_color;

	uint32_t utc = SDL_MapRGBA(RGBA_fmt, tc.r, tc.g, tc.b, tc.a);
	uint32_t unc = SDL_MapRGBA(RGBA_fmt, nc.r, nc.g, nc.b, nc.a);

	return replacePixels(utc, unc, area);

}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


bool ImagePPP::replacePixels(uint32_t target_color, uint32_t new_color)
{
	SDL_Rect area = {0, 0, w, h};
	return replacePixels(target_color, new_color, area);
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


bool ImagePPP::replacePixels(SDL_Color target_color, SDL_Color new_color)
{
	SDL_Rect area = {0, 0, w, h};
	return replacePixels(target_color, new_color, area);
}


/*===================================================================================
|																					|
|																					|
|				------------		GETTERS		  ------------						|
|																					|
|																					|
|																					|
====================================================================================*/


bool ImagePPP::show(bool inTheCurrentTarget, SDL_Rect* srcRect) 
{
	if(isInit_)
	{	
		if(isLock) 
		{
			SDL_UnlockTexture(texture);
			isLock = false;
		}

		bool restoreTarget = false;
		SDL_Texture* previousTarget = SDL_GetRenderTarget(refRen);

		if(!inTheCurrentTarget)			//si non pour la cible courrante, on force alors la copie sur le render
		{
			SDL_SetRenderTarget(refRen, nullptr);
			restoreTarget = true;
		}

		int res = SDL_RenderCopy(refRen, texture, srcRect, &myDestRect);

		if(res != 0)
		{
			sm::lastError(sm::str("||Warnig|| in ImagePPP::show() : ") 
						+ SDL_GetError());
		}


		if(restoreTarget)
			SDL_SetRenderTarget(refRen, previousTarget);

		return true;
	}

	sm::lastError("||Warnig|| in ImagePPP::show() : not yet init");
	return false;
};

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

uint32_t ImagePPP::getPixel(unsigned int x, unsigned int y) 
{
	if(isInit_)
	{
		if(x < 0 || x > w-1   ||   y < 0 || y > h-1)
			return 0;

		if(!isLock)
		{
			int locked = SDL_LockTexture(texture, nullptr, (void**) &pixels, &pitch);
			if (locked != 0)
			{
				lastError(std::string("|| fail in ImagePPP::getPixel() when trying lock texture : ") 
							+ std::string(SDL_GetError()));
				return 0;
			}
			else
			isLock = true;
		}

		return pixels[y*w + x];
	}
	
	return 0;
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


SDL_Color ImagePPP::getPixelRGBA(unsigned int x, unsigned int y)
{
	SDL_Color c;
	SDL_GetRGBA(getPixel(x, y), RGBA_fmt, &c.r, &c.g, &c.b, &c.a);
	return c;
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

SDL_Texture *ImagePPP::getTexture() 
{
	if(isLock) 
	{
		SDL_UnlockTexture(texture);
		isLock = false;
	}
	return texture;
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/


SDL_Surface* ImagePPP::toSurface()
{
	return Image::textureToSurface(texture);
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

bool ImagePPP::save(const char* format, const char* path, short quality)
{
	SDL_Surface* tmp = toSurface();
	bool success = false;


	success = Image::save(tmp, format, path, quality);
	if(!success)
		lastError(std::string("Warnig, in ImagePPP::save() : ") 
					+ std::string(SDL_GetError())); 


	if(tmp != nullptr)
		SDL_FreeSurface(tmp);

	return success;
}


/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

uint32_t ImagePPP::superimpose(uint32_t uc1, uint32_t uc2)
{
	//Cette fonction fusionne uc1 avec uc2. Si uc1 est opaque, alors on retourne uc1. 
	//uc1 est la couleur du dessus, uc2 d'en bas.

	SDL_Color c1, c2, c3;

	SDL_GetRGBA(uc1, RGBA_fmt, &c1.r, &c1.g, &c1.b, &c1.a);
	if(c1.a >= 255)
		return uc1;

	SDL_GetRGBA(uc2, RGBA_fmt, &c2.r, &c2.g, &c2.b, &c2.a);


	float pourcent_opac1 = ((float)c1.a)/255.f; //*100.f; 	// = pourcentage d'influence de chaque sous pixel
	float pourcent_opac2 = /*100.f -*/ 1 - pourcent_opac1; 	// = pourcentage d'influence de chaque sous pixel

	float r1 = (float)c1.r*pourcent_opac1;
	float g1 = (float)c1.g*pourcent_opac1;
	float b1 = (float)c1.b*pourcent_opac1;
	// float a1 = (float)c1.a*pourcent_opac1;

	float r2 = (float)c2.r*pourcent_opac2;
	float g2 = (float)c2.g*pourcent_opac2;
	float b2 = (float)c2.b*pourcent_opac2;
	// float a2 = (float)c2.a*pourcent_opac2;


	float rbuf, gbuf, bbuf, abuf;

	rbuf = r1 + r2; if(rbuf > 255) rbuf = 255;
	gbuf = g1 + g2; if(gbuf > 255) gbuf = 255;
	bbuf = b1 + b2; if(bbuf > 255) bbuf = 255;
	// abuf = a1 + a2; if(abuf > 255) abuf = 255;
	

	c3.r = static_cast<uint8_t>(std::round(rbuf));
	c3.g = static_cast<uint8_t>(std::round(gbuf));
	c3.b = static_cast<uint8_t>(std::round(bbuf));
	// c3.a = static_cast<uint8_t>(std::round(abuf));

	return SDL_MapRGBA(RGBA_fmt, c3.r, c3.g, c3.b, /*c3.a*/255);
}

/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/





/*===================================================================================
|																					|
|																					|
|																					|
|																					|
|																					|
|																					|
====================================================================================*/

	bool INTERNAL_USE_isVisibleRect(SDL_Rect rec, SDL_Rect master)
	{
		/*
		Cette fonction permet de savoir si 'rec' et 'master' possèdent une partie commune.
		*/

		struct twoInt {int x, y;};
		twoInt recLeftUp,   recRightUp,
			   recLeftDown, recRightDown;

		//inversion des roles au cas où on aurait : master < rec 
		SDL_Rect tmpRect;
		if(rec.w > master.w || rec.h > master.h)
		{
			tmpRect = master;
			master = rec;
			rec = tmpRect;
		}


		int m_y = master.y, m_x = master.x; // bord de gauche
		int m_border_x = m_x + master.w, m_border_y = m_y + master.h; //bord de droite

		if(m_border_y < m_y)
		{
			sm::swap(m_border_y, m_y);
		}
		if(m_border_x < m_x)
		{
			sm::swap(m_border_x, m_x);
		}


		int tmp = rec.x + rec.w-1;
		if(tmp < rec.x)
		{
			recLeftUp.x = tmp;
			recLeftDown.x = tmp;
			recRightUp.x = rec.x;
			recRightDown.x = rec.x;
		}
		else
		{
			recLeftUp.x = rec.x;
			recLeftDown.x = rec.x;
			recRightUp.x = tmp;
			recRightDown.x = tmp;
		}
		//=====================================
		tmp = rec.y + rec.h-1;
		if(tmp < rec.y)
		{
			recLeftUp.y = tmp;
			recRightUp.y = tmp;
			recLeftDown.y = rec.y;
			recRightDown.y = rec.y;
		}
		else
		{
			recLeftUp.y = rec.y;
			recRightUp.y = rec.y;
			recLeftDown.y = tmp;
			recRightDown.y = tmp;
		}

		if(recLeftUp.x >= m_x and recLeftUp.y >= m_y)
		{
			if(recLeftUp.x < m_border_x and recLeftUp.y < m_border_y)
				return true;
		}
		else 
		if(recLeftDown.x >= m_x and recLeftDown.y >= m_y)
		{
			if(recLeftDown.x < m_border_x and recLeftDown.y < m_border_y)
				return true;
		}
		else 
		if(recRightUp.x >= m_x and recRightUp.y >= m_y)
		{
			if(recRightUp.x < m_border_x and recRightUp.y < m_border_y)
				return true;
		}
		else 
		if(recRightDown.x >= m_x and recRightDown.y >= m_y)
		{
			if(recRightDown.x < m_border_x and recRightDown.y < m_border_y)
				return true;
		}

		
		
		return false;
	}


	bool INTERNAL_USE_isCompletlyVisibleRect(SDL_Rect rec, SDL_Rect master)
	{
		struct twoInt {int x, y;};
		twoInt recLeftUp,   recRightUp,
			   recLeftDown, recRightDown;

		int m_y = master.y, m_x = master.x;
		int m_border_x = m_x + master.w, m_border_y = m_y + master.h;

		int tmp = rec.x + rec.w-1;
		if(tmp < rec.x)
		{
			recLeftUp.x = tmp;    recLeftDown.x = tmp;
			recRightUp.x = rec.x; recRightDown.x = rec.x;
		}
		else
		{
			recLeftUp.x = rec.x; recLeftDown.x = rec.x;
			recRightUp.x = tmp;  recRightDown.x = tmp;
		}
		//=====================================
		tmp = rec.y + rec.h-1;
		if(tmp < rec.y)
		{
			recLeftUp.y = tmp;     recRightUp.y = tmp;
			recLeftDown.y = rec.y; recRightDown.y = rec.y;
		}
		else
		{
			recLeftUp.y = rec.y; recRightUp.y = rec.y;
			recLeftDown.y = tmp; recRightDown.y = tmp;
		}

		//on va consider que ce n'est completement visible que si les quatres coins le sont
		if(recLeftUp.x >= m_x && recLeftUp.x < m_border_x )
		if(recLeftDown.x >= m_x && recLeftDown.x < m_border_x )
		if(recRightUp.x >= m_x && recRightUp.x < m_border_x )
		if(recRightDown.x >= m_x && recRightDown.x < m_border_x )
		if(recLeftUp.y >= m_y && recLeftUp.y < m_border_y )
		if(recLeftDown.y >= m_y && recLeftDown.y < m_border_y )
		if(recRightUp.y >= m_y && recRightUp.y < m_border_y )
		if(recRightDown.y >= m_y && recRightDown.y < m_border_y )
			return true;

		return false;
	}


	bool INTERNAL_USE_isInvalidSrcRect(SDL_Rect& rectsrc)
	{
		if((rectsrc.w == 0 and rectsrc.h == 0) or (rectsrc.x<0 or rectsrc.y<0))
			return true;
		return false;
	}


	bool INTERNAL_USE_isInvalidDestRect(SDL_Rect& destrect)
	{
		if(destrect.w == 0 and destrect.h == 0)
			return true;
		return false;
	}




	struct vec2i{int x, y;};

	bool _1er_plusHaut_que_2e(vec2i& _prem_, vec2i& _deux_)
	{
		if(_prem_.y <= _deux_.y)
			return true;
		return false; 
	}

	bool _1er_plusBas_que_2e(vec2i& _prem_, vec2i& _deux_)
	{
		return not _1er_plusHaut_que_2e(_prem_, _deux_);
	}

	bool _1er_plusADroite_que_2e(vec2i& _prem_, vec2i& _deux_)
	{
		if(_deux_.x <= _prem_.x)
			return true;
		return false;
	}

	bool _1er_plusAGauche_que_2e(vec2i& _prem_, vec2i& _deux_)
	{
		return !_1er_plusADroite_que_2e(_prem_, _deux_); 
	}



	SDL_Rect INTERNAL_USE_cutRectForToBeAbleToDrawIt(SDL_Rect rec, SDL_Rect master)	
	{
		/*
		Cette fonction renvoie le rectangle de la partie cummune de deux rectangles 
		qui se croisent. 
		*/

				// inversion des roles au cas où on aurait : master < rec 
				// Finalement je ne sais plus si c'est nécessaire
				// SDL_Rect tmpRect;
				// if(rec.w > master.w && rec.h > master.h)
				// {
				// 	tmpRect = master;
				// 	master = rec;
				// 	rec = tmpRect;
				// }

		SDL_Rect reld={0,0,0,0};

		//définissons les coo des quatres points dans l ordre des éguilles:
		//r pour rec et m pour master 

		vec2i r1={rec.x, rec.y},               r2={rec.x+rec.w, rec.y},
			  r4={rec.x, rec.y+rec.h},    r3={rec.x+rec.w,rec.y+rec.h};   

		vec2i m1={master.x, master.y},               m2={master.x+master.w, master.y},
			  m4={master.x, master.y+master.h},    m3={master.x+master.w,master.y+master.h};   

		

		//étape 1 : calculer les coo du rect entre les deux (r.e.l.d) 
		//---------------------------------------//
		if(_1er_plusHaut_que_2e(r1, m1))
			if(_1er_plusBas_que_2e(r4, m1))
				reld.y = master.y; 

		if(_1er_plusAGauche_que_2e(r1, m1))
			if(_1er_plusADroite_que_2e(r2, m1))
				reld.x = master.x;


		//---------------------------------------//
		if(_1er_plusHaut_que_2e(m1, r1))
			if(_1er_plusHaut_que_2e(r1, m4))
				reld.y = rec.y;

		if(_1er_plusADroite_que_2e(r1, m1))
			if(_1er_plusAGauche_que_2e(r1, m2))
				reld.x = rec.x;
		//---------------------------------------//










		//étape 2 : les dimensions
		if(_1er_plusHaut_que_2e(r1, m1))
			if(_1er_plusBas_que_2e(r4, m1))
				reld.h = rec.h+rec.y-reld.y;

		if(_1er_plusAGauche_que_2e(r1, m1))
			if(_1er_plusADroite_que_2e(r2, m1))
				reld.w = rec.x+rec.w-reld.x; //ic reld.x = déjà master.x	
		


		//---------------------------------------//
		if(_1er_plusHaut_que_2e(m1, r1))
			if(_1er_plusHaut_que_2e(r1, m4))
			{
				if(_1er_plusHaut_que_2e(r4, m4))
					reld.h = rec.h;   
				else if(_1er_plusBas_que_2e(r4, m4))
					reld.h = rec.h - ((rec.y+rec.h) - (master.y+master.h));
			}
			
		if(_1er_plusADroite_que_2e(r1, m1))
			if(_1er_plusAGauche_que_2e(r1, m2))
			{
				if(_1er_plusAGauche_que_2e(r2, m2))
					reld.w = rec.w;   
				else if(_1er_plusADroite_que_2e(r2, m2))
					reld.w =  rec.w - ((rec.x+rec.w) - (master.x+master.w));
			} 



		/*
		J'ai trouvé les solutions par tâtonnement en faisant des schémas sur une feuille. Le tout
		semble fonctionner correctement jusqu'à preuve du contraire. 
		Le code n'est peut-être pas le plus optimisé mais c'est ce que j'ai à offrir pour le moment.
		*/


		//des corrections de fin 
		if(reld.w > master.w)
			reld.w -= reld.w-master.w;

		if(reld.h> master.h)
			reld.h -= reld.h-master.h;


		return reld;


	}

	bool INTERNAL_USE_isInTheGrid(int x, int y, SDL_Rect grid)
	{
		int min_index_x = grid.x;
		int min_index_y = grid.y;
		int max_index_x = grid.x + grid.w -1; 
		int max_index_y = grid.y + grid.h -1; 

		//indice max est inclu dans les indices valides

		if(sm::isInInterval_in_in(min_index_x, max_index_x, x))
			if(sm::isInInterval_in_in(min_index_y, max_index_y, y))
				return true;

		return false;
	}



}//namespace sm fin
