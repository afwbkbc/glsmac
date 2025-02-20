#pragma once

#include "gse/program/Types.h"

namespace gse {
namespace value {

class LoopControl : public Value {
public:

	static const type_t GetType() { return Value::T_LOOPCONTROL; }

	LoopControl( gc::Space* const gc_space, const program::loop_control_type_t initial_value )
		: Value( gc_space, GetType() )
		, value( initial_value ) {
	}

	program::loop_control_type_t value;
};

}
}
