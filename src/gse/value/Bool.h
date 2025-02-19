#pragma once

#include "gse/Value.h"

namespace gse {
namespace value {

class Bool : public Value {
public:

	static const type_t GetType() { return Value::T_BOOL; }

	Bool( const bool initial_value )
		: Value( GetType() )
		, value( initial_value ) {
	}

	bool value;
};

}
}
