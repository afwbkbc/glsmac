#include "Variable.h"

namespace gse {
namespace program {

Variable::Variable( const si_t& si, const std::string& name, const variable_hints_t hints )
	: Operand( si, OT_VARIABLE )
	, name( name )
	, hints( hints ) {}

const std::string Variable::ToString() const {
	return name;
}
const std::string Variable::Dump( const size_t depth ) const {
	return Formatted( "Variable" + m_si.ToString() + "( " + name + " )", depth );
}

}
}
