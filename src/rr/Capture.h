#pragma once

#include "RR.h"

namespace rr {
	
CLASS( Capture, RR )
	
	typedef uint32_t data_t;
	
	static const type_t Type() {
		return RR_GETDATA;
	}
	Capture() : RR( Type() ) {}

	// request
	size_t screen_x;
	size_t screen_inverse_y;
	
	// response
	data_t data;

};
	
}
