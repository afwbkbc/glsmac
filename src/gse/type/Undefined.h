#pragma once

#include "Type.h"

namespace gse {
namespace type {

class Undefined : public Type {
public:
	Undefined()
		: Type( Type::T_UNDEFINED ) {}

	Undefined( const Undefined& other )
		: Undefined() {}
};

}
}
