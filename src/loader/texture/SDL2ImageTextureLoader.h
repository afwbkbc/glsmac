#pragma once

#include "TextureLoader.h"

#include <unordered_map>
#include <SDL_image.h>

using namespace std;

namespace loader {
namespace texture {

MINOR_CLASS( SDL2Image, TextureLoader )
	void Start();
	void Stop();
	void Iterate();

	types::Texture *LoadTexture( const string &name );
	types::Texture *LoadTexture( const string &name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value = 1.0 );
	
protected:
	// cache all textures for future use
	typedef unordered_map< string, types::Texture* > texture_map_t;
	texture_map_t m_textures;
	texture_map_t m_subtextures;

};

} /* namespace texture */
} /* namespace loader */
