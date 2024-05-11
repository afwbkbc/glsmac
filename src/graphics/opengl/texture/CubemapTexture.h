#pragma once

#include "Texture.h"

namespace graphics {
namespace opengl {

CLASS( CubemapTexture, Texture )
	CubemapTexture( types::texture::Texture* texture );
	void Load() override;
};

}
}
