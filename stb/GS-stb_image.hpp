#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace gs
{

class STB_Image
{
	public :

	STB_Image() = default;

	~STB_Image()
	{
		freeImg();
	}


	bool loadImg(const char* path)
	{
		m_pixels = stbi_load(path, &m_w, &m_h, &m_channels_nb, 0);

		return m_pixels != nullptr;
	}

	void freeImg()
	{
		if(m_pixels != nullptr)
			stbi_image_free(m_pixels);

		m_pixels = nullptr;
		m_w = 0;
		m_h = 0;
		m_channels_nb = 0;
	}

	uint32_t getPixel(int x, int y)
	{
		if(m_pixels == nullptr)
		{
			gs::lastError("||Warning|| in 'STB_Image::getPixel()', it seems that no image is loeaded, because of 'm_pixels == nullptr' is true. Try 'STB_Image::loadImg()' before. ");
			return 0;
		}

		if(x < 0 or y < 0 or x >= m_w or y >= m_h)
		{
			gs::lastError(std::string("||Warning|| in 'STB_Image::getPixel()', invalid pixel coordinates. [Given x : ")+std::to_string(x)+std::string(", given y : ")+std::to_string(y)+std::string("] ")); 
			return 0;
		}

		
		switch(m_channels_nb)
		{
			case 4: return gs::u32RGBA(
				m_pixels[ (y*m_w +x)*m_channels_nb +0], 
				m_pixels[ (y*m_w +x)*m_channels_nb +1],
				m_pixels[ (y*m_w +x)*m_channels_nb +2], 
				m_pixels[ (y*m_w +x)*m_channels_nb +3]);

			case 3: return gs::u32RGBA(
				m_pixels[ (y*m_w +x)*m_channels_nb +0], 
				m_pixels[ (y*m_w +x)*m_channels_nb +1],
				m_pixels[ (y*m_w +x)*m_channels_nb +2], 0);

			case 2: return gs::u32RGBA(
				m_pixels[ (y*m_w +x)*m_channels_nb +0],
				m_pixels[ (y*m_w +x)*m_channels_nb +1], 0, 0);

			case 1: return gs::u32RGBA(
				m_pixels[ (y*m_w +x)*m_channels_nb +0], 0, 0, 0);
			
			default: break;
		}

		return 0;
	}

	const unsigned char* getAllPixels() const {return m_pixels;} 

	int getW() const {return m_w;}
	int getH() const {return m_h;}
	int getChannelsNb() const {return m_channels_nb;}
	

	protected :

		int m_w=0;
		int m_h=0;
		int m_channels_nb=0;
		unsigned char* m_pixels; 

};

}