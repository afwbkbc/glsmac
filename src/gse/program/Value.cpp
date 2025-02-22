#include "Value.h"

namespace gse {
namespace program {

Value::Value( const si_t& si, gse::Value* const value )
	: Operand( si, OT_VALUE )
	, value( value ) {}

const std::string Value::ToString() const {
	return value
		? value->ToString()
		: "nothing";
}
const std::string Value::Dump( const size_t depth ) const {
	return Formatted( "Value" + m_si.ToString() + "( " + ToString() + " )", depth );
}

}
}
