#pragma once

#include "gse/Value.h"

namespace gse {
namespace value {

class Bool : public Value {
public:

	static const type_t GetType() { return Value::T_BOOL; }

	Bool( gc::Space* const gc_space, const bool initial_value )
		: Value( gc_space, GetType() )
		, value( initial_value ) {
	}

	bool value;

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override {
		return "gse::value::Bool( " + std::to_string( value ) + " )";
	}
#endif

};

}
}
