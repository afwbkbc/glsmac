#include "Nothing.h"

namespace gse {
namespace program {

Nothing::Nothing( const si_t& si )
	: Operand( si, OT_NOTHING ) {}

const std::string Nothing::Dump( const size_t depth ) const {
	return Formatted( "Nothing" + m_si.ToString() + "()", depth );
}

}
}
