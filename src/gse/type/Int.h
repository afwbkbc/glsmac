#pragma once

#include <cstdint>

#include "Type.h"

namespace gse {
namespace type {

class Int : public Type {
public:
	Int( const int32_t initial_value )
		: Type( Type::T_INT )
		, value( initial_value ) {}

	int32_t value;

	Int( const Int& other )
		: Int( other.value ) {}

};

}
}
