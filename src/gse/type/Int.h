#pragma once

#include <cstdint>

#include "Type.h"

namespace gse {
namespace type {

class Int : public Type {
public:

	static const type_t GetType() { return Type::T_INT; }

	Int( const int64_t initial_value )
		: Type( GetType() )
		, value( initial_value ) {}

	int64_t value;

	Int( const Int& other )
		: Int( other.value ) {}

};

}
}
