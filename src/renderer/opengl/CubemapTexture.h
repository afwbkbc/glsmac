#pragma once

#include "Texture.h"

namespace renderer {
namespace opengl {

class CubemapTexture : public Texture {
public:
	CubemapTexture( types::Texture *texture );
	void Load();
};

} /* namespace opengl */
} /* namespace renderer */
