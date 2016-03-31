#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <string>
#include <stdint.h>
typedef int TextureHandle;
namespace gfx {
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
	GLuint GetHandle();
	void Apply( GLuint location, int index );
	bool GetLoaded();
	void SetFilename( const char* filename );
	std::string GetFilename();
	uint64_t GetAddress();

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
	GLuint m_Handle;
	int m_Width, m_Height, m_Channels;
	bool m_Loaded;
	std::string m_Filename;
	TextureType m_Type;
};
}
