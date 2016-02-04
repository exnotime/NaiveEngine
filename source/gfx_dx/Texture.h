#pragma once
#include <stdio.h>
#include <string>

typedef int TextureHandle;
namespace gfx_dx {
enum TextureType {
	TEXTURE_COLOR,
	TEXTURE_GREYSCALE,
	TEXTURE_CUBE
};

class Texture {
  public:
	Texture();
	~Texture();

	bool Init( const char* Filename, TextureType type);
	void Apply();
	bool GetLoaded();
	void SetFilename( const char* filename );
	std::string GetFilename();
	float GetWidth() {
		return (float)m_Width;
	}
	float GetHeight() {
		return (float)m_Height;
	}
	int GetChannels() {
		return m_Channels;
	}
	void Resize( int width, int height );
  private:
	int m_Width, m_Height, m_Channels;
	bool m_Loaded;
	std::string m_Filename;
	TextureType m_Type;
};
}
