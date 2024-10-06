#include "LoopControl.h"

#include "common/Assert.h"

namespace gse {
namespace program {

LoopControl::LoopControl( const si_t& si, const loop_control_type_t loop_control_type )
	: Operand( si, OT_LOOP_CONTROL )
	, loop_control_type( loop_control_type ) {}

const std::string LoopControl::ToString() const {
	switch ( loop_control_type ) {
		case LCT_BREAK:
			return "break";
		case LCT_CONTINUE:
			return "continue";
		default:
			THROW( "unexpected control type: " + std::to_string( loop_control_type ) );
	}
}
const std::string LoopControl::Dump( const size_t depth ) const {
	return Formatted( "LoopControl" + m_si.ToString() + "( " + ToString() + " )", depth );
}

}
}
