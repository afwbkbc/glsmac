#pragma once

#include <string>

#include "../Loader.h"

#include "types/Texture.h"

namespace loader {
namespace texture {

MAJOR_CLASS( TextureLoader, Loader )
	virtual types::Texture *LoadTexture( const std::string &name ) = 0;
};

} /* namespace texture */
} /* namespace loader */
