#pragma once

#include "RR.h"

/**
 * Captures rendered mesh into texture
 */

namespace types::texture {
class Texture;
}

namespace scene {
class Camera;
}

namespace rr {

CLASS( Capture, RR )

	typedef uint32_t data_t;

	static const type_t Type() {
		return RR_CAPTURE;
	}
	Capture()
		: RR( Type() ) {}

	// request
	scene::Camera* camera = nullptr;
	size_t texture_width = 0;
	size_t texture_height = 0;

	// response
	types::texture::Texture* texture = nullptr;

	// cleanup
	~Capture();
};

}
