#pragma once

#include <string>

#include "TextureLoader.h"

#include "types/texture/Texture.h"

namespace loader {
namespace texture {

CLASS( Null, TextureLoader )
	types::texture::Texture* LoadTextureImpl( const std::string& filename ) override { return nullptr; }
	types::texture::Texture* LoadTextureImpl( const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags = ui::LT_NONE, const float value = 1.0 ) override { return nullptr; }
};

}
}
