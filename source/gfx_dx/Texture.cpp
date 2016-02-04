#include "Texture.h"
#include <Soil2/SOIL2.h>

using namespace gfx_dx;

Texture::Texture() {
	m_Loaded = false;
	m_Width = 0;
	m_Height = 0;
	m_Channels = 0;
	m_Filename = "";
}

Texture::~Texture() {
}

bool Texture::Init(const char* Filename, TextureType type) {
	m_Filename = std::string(Filename);
	m_Type = type;

	if (type == TEXTURE_COLOR || type == TEXTURE_GREYSCALE) {
		int forceChannel = type == TEXTURE_COLOR ? 4 : 1;
	} else if (type == TEXTURE_CUBE) {
	}

	//Logger::Log( pString( "Loaded texture: " ) + Filename, "Texture", LogSeverity::DEBUG_MSG );
	return true;
}

void Texture::Apply() {
}

bool Texture::GetLoaded() {
	return m_Loaded;
}

void Texture::SetFilename(const char* filename) {
	m_Filename = std::string(filename);
}

std::string Texture::GetFilename() {
	return m_Filename;
}

void Texture::Resize(int width, int height) {
}
