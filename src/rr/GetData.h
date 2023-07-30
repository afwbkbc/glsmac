#pragma once

#include <cstdint>

#include "RR.h"

/**
 * Requests data from data mesh at window coordinates
 */

namespace rr {

CLASS( GetData, RR )
	
	typedef uint32_t data_t;
	
	static const type_t Type() {
		return RR_GETDATA;
	}
	GetData() : RR( Type() ) {}

	// request
	size_t screen_x;
	size_t screen_inverse_y;
	
	// response
	data_t data;

};
	
}
