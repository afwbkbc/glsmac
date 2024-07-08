#pragma once

#include "Type.h"

#include "gse/program/Types.h"

namespace gse {
namespace type {

class LoopControl : public Type {
public:

	static const type_t GetType() { return Type::T_LOOPCONTROL; }

	LoopControl( const program::loop_control_type_t initial_value )
		: Type( GetType() )
		, value( initial_value ) {
	}

	program::loop_control_type_t value;
};

}
}
