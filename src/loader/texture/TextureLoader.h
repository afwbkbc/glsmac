#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>

#include "../Loader.h"

#include "types/Texture.h"
#include "types/Color.h"

using namespace types;

namespace loader {
namespace texture {

CLASS( TextureLoader, Loader )

	// loadtexture flags
	static constexpr uint8_t LT_NONE = 0;
	static constexpr uint8_t LT_ROTATE = 1;
	static constexpr uint8_t LT_FLIPV = 2;
	static constexpr uint8_t LT_FLIPH = 4;
	static constexpr uint8_t LT_TILED = 8;
	static constexpr uint8_t LT_ALPHA = 16;
	static constexpr uint8_t LT_CONTRAST = 32;

	typedef std::unordered_set< Color::rgba_t > transparent_colors_t;
	
	// load full texture
	virtual Texture* LoadTexture( const std::string &name ) = 0;
	Texture* LoadTextureTCs( const std::string &name, const transparent_colors_t transparent_colors );
	Texture* LoadTextureTC( const std::string &name, const Color::rgba_t transparent_color );

	// load part of texture
	virtual Texture* LoadTexture( const std::string &name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags = LT_NONE, const float value = 1.0 ) = 0;
	Texture* LoadTextureTCs( const std::string &name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const transparent_colors_t transparent_colors, const uint8_t flags = LT_NONE, const float value = 1.0 );
	Texture* LoadTextureTC( const std::string &name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const Color::rgba_t transparent_color, const uint8_t flags, const float value );
	
	const Texture* GetColorTexture( const Color& color );
	
	// treat specific color as transparent
	void SetTransparentColor( const Color::rgba_t rgba );
	
protected:
	transparent_colors_t m_transparent_colors;
	bool m_is_transparent_color_set = false;
	
	typedef std::unordered_map< Color::rgba_t, const Texture* > color_texture_map_t;
	color_texture_map_t m_color_textures = {};
	
};

} /* namespace texture */
} /* namespace loader */
