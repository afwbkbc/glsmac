#include "Value.h"

namespace gse {
namespace program {

Value::Value( const si_t& si, const gse::Value& value )
	: Operand( si, OT_VALUE )
	, value( value ) {}

const std::string Value::ToString() const {
	return value.ToString();
}
const std::string Value::Dump( const size_t depth ) const {
	return Formatted( "Value" + m_si.ToString() + "( " + value.ToString() + " )", depth );
}

}
}
