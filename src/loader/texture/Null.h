#pragma once

#include <string>

#include "TextureLoader.h"

#include "types/Texture.h"

namespace loader {
namespace texture {

CLASS( Null, TextureLoader )
	Texture* LoadTextureImpl( const std::string& name ) override { return nullptr; }
	Texture* LoadTextureImpl( const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags = LT_NONE, const float value = 1.0 ) override { return nullptr; }
};

}
}
