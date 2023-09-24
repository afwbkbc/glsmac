#pragma once

#include "Texture.h"

namespace graphics {
namespace opengl {

CLASS( CubemapTexture, Texture )
	CubemapTexture( types::Texture* texture );
	void Load() override;
};

} /* namespace opengl */
} /* namespace graphics */
