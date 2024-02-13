#pragma once

#include <cstdint>

#include "Type.h"

namespace gse {
namespace type {

class Float : public Type {
public:

	static const type_t GetType() { return Type::T_FLOAT; }

	Float( const float initial_value )
		: Type( GetType() )
		, value( initial_value ) {}

	float value;

	Float( const Float& other )
		: Float( other.value ) {}

};

}
}
