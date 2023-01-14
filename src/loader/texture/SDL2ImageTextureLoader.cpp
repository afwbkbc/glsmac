#include "SDL2ImageTextureLoader.h"

using namespace std;

namespace loader {
namespace texture {

void SDL2ImageTextureLoader::Start() {

}

void SDL2ImageTextureLoader::Stop() {

}

void SDL2ImageTextureLoader::Iterate() {

}

types::Texture *SDL2ImageTextureLoader::LoadTexture( const string &name ) {

	texture_map_t::iterator it = m_textures.find( name );
	if (it != m_textures.end()) {
		return it->second;
	}
	else {
		Log( "Loading texture \"" + name + "\"" );
		SDL_Surface *image = IMG_Load( ( GetRoot() + name ).c_str() );
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

		NEWV( texture, types::Texture, name );
		texture->m_width = image->w;
		texture->m_height = image->h;
		texture->m_aspect_ratio = (float) texture->m_height / texture->m_width;
		texture->m_bpp = image->format->BitsPerPixel / 8;
		texture->m_bitmap_size = image->w * image->h * texture->m_bpp;
		texture->m_bitmap = (unsigned char*) malloc( texture->m_bitmap_size );
		memcpy( texture->m_bitmap, image->pixels, texture->m_bitmap_size );
		SDL_FreeSurface(image);

		if ( m_is_transparent_color_set ) {
			for (size_t i = 0 ; i < texture->m_bitmap_size ; i += texture->m_bpp) {
				if (!memcmp( texture->m_bitmap + i, &m_transparent_color, texture->m_bpp )) {
					memset( texture->m_bitmap + i, 0, texture->m_bpp );
				}
			}
		}
		
		m_textures[name] = texture;
		
		DEBUG_STAT_INC( textures_loaded );
		
		return texture;
	}

}

types::Texture *SDL2ImageTextureLoader::LoadTexture( const string &name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value ) {
	
	const string subtexture_key =
		name + ":" +
		to_string( x1 ) + ":" +
		to_string( y1 ) + ":" +
		to_string( x2 ) + ":" +
		to_string( y2 ) + ":" +
		to_string(flags) + ":" +
		to_string(value)
	;
	
	texture_map_t::iterator it = m_subtextures.find( subtexture_key );
	if (it != m_subtextures.end()) {
		return it->second;
	}
	else {
	
		auto *full_texture = LoadTexture( name );

		NEWV( subtexture, types::Texture, subtexture_key );
		
		subtexture->CopyFrom(full_texture, x1, y1, x2, y2);
		if ((flags & LT_ROTATE) == LT_ROTATE) {
			subtexture->Rotate();
		}
		if ((flags & LT_FLIPV) == LT_FLIPV) {
			subtexture->FlipV();
		}
		
		if ((flags & LT_ALPHA) == LT_ALPHA) {
			subtexture->SetAlpha(value);
		}
		if ((flags & LT_CONTRAST) == LT_CONTRAST) {
			subtexture->SetContrast(value);
		}
		
		if ((flags & LT_TILED) == LT_TILED) {
			subtexture->m_is_tiled = true;
		}
		
		m_subtextures[subtexture_key] = subtexture;

		return subtexture;
	}
}

} /* namespace texture */
} /* namespace loader */
