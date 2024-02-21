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
	Texture* LoadTexture( const std::string& name );

	// load part of texture
	Texture* LoadTexture( const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags = LT_NONE, const float value = 1.0 );

	// create texture of solid color
	Texture* GetColorTexture( const Color& color );

protected:

	virtual Texture* LoadTextureImpl( const std::string& name ) = 0;
	virtual Texture* LoadTextureImpl( const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value ) = 0;

	transparent_colors_t m_transparent_colors;

	typedef std::unordered_map< Color::rgba_t, Texture* > color_texture_map_t;
	color_texture_map_t m_color_textures = {};

private:
	const transparent_colors_t& GetTCs( const std::string& name );

};

} /* namespace texture */
} /* namespace loader */
