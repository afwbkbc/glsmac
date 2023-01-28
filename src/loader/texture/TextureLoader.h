#pragma once

#include <string>

#include "../Loader.h"

#include "types/Texture.h"

using namespace types;

namespace loader {
namespace texture {

CLASS( TextureLoader, Loader )

	// loadtexture flags
	static const uint8_t LT_NONE = 0;
	static const uint8_t LT_ROTATE = 1;
	static const uint8_t LT_FLIPV = 2;
	static const uint8_t LT_FLIPH = 4;
	static const uint8_t LT_TILED = 8;
	static const uint8_t LT_ALPHA = 16;
	static const uint8_t LT_CONTRAST = 32;

	// load full texture
	virtual Texture* LoadTexture( const std::string &name ) = 0;
	Texture* LoadTexture( const std::string &name, const Color::rgba_t transparent_color );

	// load part of texture
	virtual Texture* LoadTexture( const string &name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags = LT_NONE, const float value = 1.0 ) = 0;
	
	// treat specific color as transparent
	void SetTransparentColor( const Color::rgba_t rgba );
	
protected:
	Color::rgba_t m_transparent_color;
	bool m_is_transparent_color_set = false;
	
};

} /* namespace texture */
} /* namespace loader */
