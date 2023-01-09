#pragma once

#include "Vec2.h"

namespace types {

template <typename DATATYPE = float>
class Vec22 {
public:
	Vec2<DATATYPE> vec1;
	Vec2<DATATYPE> vec2;
};

} /* namespace types */
