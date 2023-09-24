#pragma once

#include "Vec2.h"

namespace types {

template< typename DATATYPE = float >
class Vec22 {
public:
	Vec2< DATATYPE > vec1 = {
		0,
		0
	};

	Vec2< DATATYPE > vec2 = {
		0,
		0
	};
};

} /* namespace types */
