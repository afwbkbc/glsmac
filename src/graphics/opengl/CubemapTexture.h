#pragma once

#include "Texture.h"

namespace graphics {
namespace opengl {

class CubemapTexture : public Texture {
public:
	CubemapTexture( types::Texture *texture );
	void Load();
};

} /* namespace opengl */
} /* namespace graphics */
