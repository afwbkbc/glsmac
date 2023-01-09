#include "SDL2ImageTextureLoader.h"

namespace loader {
namespace texture {

void SDL2ImageTextureLoader::Start() {

}

void SDL2ImageTextureLoader::Stop() {

}

void SDL2ImageTextureLoader::Iterate() {

}

types::Texture *SDL2ImageTextureLoader::LoadTexture( const std::string &name ) {

	Log( "Loading texture \"" + name + "\"" );
	SDL_Surface *image = IMG_Load( ( /*m_textures_directory*/ GetRoot() + name ).c_str() );
	if (!image) {
		throw TextureLoaderError( IMG_GetError() );
	}
	if (image->format->format != SDL_PIXELFORMAT_RGBA32) {
		// we must have all images in same format
		SDL_Surface *old = image;
		image = SDL_ConvertSurfaceFormat(old, SDL_PIXELFORMAT_RGBA32, 0);
		if (!image) {
			throw TextureLoaderError( IMG_GetError() );
		}
		SDL_FreeSurface(old);
	}
	
	auto *texture = new types::Texture( name );
	texture->m_width = image->w;
	texture->m_height = image->h;
	texture->m_bpp = image->format->BitsPerPixel / 8;
	texture->m_bitmap_size = image->w * image->h * texture->m_bpp;
	texture->m_bitmap = new unsigned char[ texture->m_bitmap_size ];
	memcpy( texture->m_bitmap, image->pixels, texture->m_bitmap_size );
	SDL_FreeSurface(image);
	
	return texture;

}

} /* namespace texture */
} /* namespace loader */
