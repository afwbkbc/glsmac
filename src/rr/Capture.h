#pragma once

#include "RR.h"

#include "types/Texture.h"
#include "scene/Camera.h"

/**
 * Captures rendered mesh into texture
 */

namespace rr {

CLASS( Capture, RR )
	
	typedef uint32_t data_t;
	
	static const type_t Type() {
		return RR_CAPTURE;
	}
	Capture() : RR( Type() ) {}

	// request
	scene::Camera* camera = nullptr;
	size_t texture_width = 0;
	size_t texture_height = 0;
	
	// response
	types::Texture* texture = nullptr;

	~Capture() {
		if ( camera ) {
			DELETE( camera );
		}
		if ( texture ) {
			DELETE( texture );
		}
	}
};

}
