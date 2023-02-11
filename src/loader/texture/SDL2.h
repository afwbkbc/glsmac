#pragma once

#include <unordered_map>
#include <SDL_image.h>

#include "TextureLoader.h"

using namespace std;

namespace loader {
namespace texture {

CLASS( SDL2, TextureLoader )
	virtual ~SDL2();
	
	void Start();
	void Stop();
	void Iterate();

	Texture* LoadTexture( const string &name );
	Texture* LoadTexture( const string &name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value = 1.0 );
	
protected:
	// cache all textures for future use
	typedef unordered_map< string, Texture* > texture_map_t;
	texture_map_t m_textures = {};
	texture_map_t m_subtextures = {};

};

} /* namespace texture */
} /* namespace loader */
