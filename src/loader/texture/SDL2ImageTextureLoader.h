#pragma once

#include "TextureLoader.h"

#include <unordered_map>
#include <SDL_image.h>

using namespace std;

namespace loader {
namespace texture {

MINOR_CLASS( SDL2Image, TextureLoader )
	SDL2ImageTextureLoader( const string & textures_directory ) : m_textures_directory( textures_directory ) { } ;
	void Start();
	void Stop();
	void Iterate();

	const types::Texture *LoadTexture( const string &name );
	
protected:
	const string m_textures_directory;
	
	// cache all textures for future use
	typedef unordered_map< string, const types::Texture* > texture_map_t;
	texture_map_t m_textures;

};

} /* namespace texture */
} /* namespace loader */
