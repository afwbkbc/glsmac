#include "SDL2.h"

#include "util/FS.h"
#include "types/texture/Texture.h"

namespace loader {
namespace texture {

SDL2::~SDL2() {
	for ( auto& it : m_textures ) {
		DELETE( it.second );
	}
	for ( auto& it : m_subtextures ) {
		DELETE( it.second );
	}
}

void SDL2::Start() {

}

void SDL2::Stop() {

}

void SDL2::Iterate() {

}

types::texture::Texture* SDL2::LoadTextureImpl( const std::string& filename ) {

	texture_map_t::iterator it = m_textures.find( filename );
	if ( it != m_textures.end() ) {
		return it->second;
	}
	else {
		Log( "Loading texture \"" + filename + "\"" );
		auto* image = IMG_Load( filename.c_str() );
		ASSERT( image, IMG_GetError() );
		if ( image->format->format != SDL_PIXELFORMAT_RGBA32 ) {
			// we must have all images in same format
			SDL_Surface * old = image;
			image = SDL_ConvertSurfaceFormat( old, SDL_PIXELFORMAT_RGBA32, 0 );
			ASSERT( image, IMG_GetError() );
			SDL_FreeSurface( old );
		}

		NEWV( texture, types::texture::Texture, filename, image->w, image->h );
		texture->SetBitmap( image->pixels );
		ASSERT( texture->m_bpp == image->format->BitsPerPixel / 8, "unsupported texture bpp" );
		memcpy( ptr( texture->m_bitmap, 0, texture->m_bitmap_size ), image->pixels, texture->m_bitmap_size );
		SDL_FreeSurface( image );

		FixTexture( texture ); // some pcx files have strange artifacts that we need to fix procedurally

		FixTransparency( texture );
		FixYellowShadows( texture );

		m_textures.insert(
			{
				filename,
				texture
			}
		);

		DEBUG_STAT_INC( textures_loaded );

		return texture;
	}

}

types::texture::Texture* SDL2::LoadTextureImpl( const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value ) {
	ASSERT( x1 <= x2, "LoadTexture x overflow ( " + std::to_string( x1 ) + " > " + std::to_string( x2 ) + " )" );
	ASSERT( y1 <= y2, "LoadTexture y overflow ( " + std::to_string( y1 ) + " > " + std::to_string( y2 ) + " )" );

	const std::string subtexture_key =
		name + ":" +
			std::to_string( x1 ) + ":" +
			std::to_string( y1 ) + ":" +
			std::to_string( x2 ) + ":" +
			std::to_string( y2 ) + ":" +
			std::to_string( flags ) + ":" +
			std::to_string( value );

	texture_map_t::iterator it = m_subtextures.find( subtexture_key );
	if ( it != m_subtextures.end() ) {
		return it->second;
	}
	else {

		auto* full_texture = LoadTextureImpl( name );

		NEWV( subtexture, types::texture::Texture, subtexture_key, x2 - x1 + 1, y2 - y1 + 1 );

		subtexture->AddFrom( full_texture, types::texture::AM_DEFAULT, x1, y1, x2, y2 );
		if ( ( flags & ui_legacy::LT_ROTATE ) == ui_legacy::LT_ROTATE ) {
			subtexture->Rotate();
		}
		if ( ( flags & ui_legacy::LT_FLIPV ) == ui_legacy::LT_FLIPV ) {
			subtexture->FlipV();
		}

		if ( ( flags & ui_legacy::LT_ALPHA ) == ui_legacy::LT_ALPHA ) {
			subtexture->SetAlpha( value );
		}
		if ( ( flags & ui_legacy::LT_CONTRAST ) == ui_legacy::LT_CONTRAST ) {
			subtexture->SetContrast( value );
		}

		if ( ( flags & ui_legacy::LT_TILED ) == ui_legacy::LT_TILED ) {
			subtexture->m_is_tiled = true;
		}

		// needed?
		FixTransparency( subtexture );
		//FixYellowShadows( subtexture );

		m_subtextures[ subtexture_key ] = subtexture;

		return subtexture;
	}
}

void SDL2::FixTransparency( types::texture::Texture* texture ) const {
	if ( !m_transparent_colors.empty() ) {
		void* at = nullptr;
		for ( size_t i = 0 ; i < texture->m_bitmap_size ; i += texture->m_bpp ) {
			at = ptr( texture->m_bitmap, i, texture->m_bpp );
			for ( auto& c : m_transparent_colors ) {
				if ( !memcmp( at, &c, texture->m_bpp ) ) {
					memset( at, 0, texture->m_bpp );
					break;
				}
			}
		}
	}
}

static const types::Color::rgba_t s_yellow_shadow_src = types::Color::RGB( 253, 189, 118 );
static const types::Color::rgba_t s_yellow_shadow_dst = types::Color::RGBA( 0, 0, 0, 127 );

void SDL2::FixYellowShadows( types::texture::Texture* texture ) const {
	if ( m_fix_yellow_shadows ) {
		void* at = nullptr;
		for ( size_t i = 0 ; i < texture->m_bitmap_size ; i += texture->m_bpp ) {
			at = ptr( texture->m_bitmap, i, texture->m_bpp );
			if ( !memcmp( at, &s_yellow_shadow_src, texture->m_bpp ) ) {
				memcpy( at, &s_yellow_shadow_dst, texture->m_bpp );
			}
		}
	}
}

void SDL2::FixTexture( types::texture::Texture* texture ) const {
	if ( texture->m_name == "interface.pcx" ) {
		ASSERT( texture->m_width == 750 && texture->m_height == 900, "unexpected texture.pcx dimensions" );
		// rain icons have weird brown lines on them
		const std::vector< types::Vec2< size_t > > pixels_to_fix = {
			{ 725, 309 },
			{ 724, 310 },
			{ 723, 310 },
			{ 722, 310 },
			{ 721, 311 },
			{ 720, 312 },
			{ 719, 312 },
		};
		for ( auto& v : pixels_to_fix ) {
			texture->SetPixel( v.x, v.y, texture->GetPixel( v.x + 1, v.y ) );
		}
	}
}

}
}
