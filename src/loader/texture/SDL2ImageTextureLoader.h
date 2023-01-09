#pragma once

#include <SDL_image.h>

#include "TextureLoader.h"

namespace loader {
namespace texture {

MINOR_CLASS( SDL2Image, TextureLoader )
	SDL2ImageTextureLoader( const std::string & textures_directory ) : m_textures_directory( textures_directory ) { } ;
	void Start();
	void Stop();
	void Iterate();

	types::Texture *LoadTexture( const std::string &name );
protected:
	const std::string m_textures_directory;


};

} /* namespace texture */
} /* namespace loader */
