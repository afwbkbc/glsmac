#pragma once

#include <cstdint>

#include "gse/Value.h"

namespace gse {
namespace value {

class Int : public Value {
public:

	static const type_t GetType() { return Value::T_INT; }

	Int( gc::Space* const gc_space, const int64_t initial_value )
		: Value( gc_space, GetType() )
		, value( initial_value ) {}

	int64_t value;

	Int( gc::Space* const gc_space, const Int& other )
		: Int( gc_space, other.value ) {}

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override {
		return "gse::value::Int( " + std::to_string( value ) + " )";
	}
#endif

};

}
}
