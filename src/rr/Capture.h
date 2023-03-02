#pragma once

#include "RR.h"

#include "types/Texture.h"

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
	
	// response
	types::Texture* texture = nullptr;

};
	
}
