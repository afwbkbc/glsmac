#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>

#include "loader/Loader.h"

#include "ui/theme/Types.h"

#include "types/Color.h"

namespace types::texture {
class Texture;
}

namespace loader {
namespace texture {

CLASS( TextureLoader, Loader )

	typedef std::unordered_set< types::Color::rgba_t > transparent_colors_t;

	// load full texture
	types::texture::Texture* LoadTexture( const resource::resource_t res );
	types::texture::Texture* LoadCustomTexture( const std::string& filename );

	// load part of texture
	types::texture::Texture* LoadTexture( const resource::resource_t res, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags = ui::LT_NONE, const float value = 1.0 );

	// create texture of solid color
	types::texture::Texture* GetColorTexture( const types::Color& color );

protected:

	virtual types::texture::Texture* LoadTextureImpl( const std::string& filename ) = 0;
	virtual types::texture::Texture* LoadTextureImpl( const std::string& filename, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value ) = 0;

	transparent_colors_t m_transparent_colors = {};
	bool m_fix_yellow_shadows = false;

	typedef std::unordered_map< types::Color::rgba_t, types::texture::Texture* > color_texture_map_t;
	color_texture_map_t m_color_textures = {};

private:
	const transparent_colors_t& GetTCs( const resource::resource_t res );

};

}
}
