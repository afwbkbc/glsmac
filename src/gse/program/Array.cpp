#include "Array.h"

#include "Expression.h"

namespace gse {
namespace program {

Array::Array( const si_t& si, const elements_t& elements )
	: Operand( si, OT_ARRAY )
	, elements( elements ) {}

Array::~Array() {
	for ( auto& it : elements ) {
		delete it;
	}
}

const std::string Array::ToString() const {
	std::string args = "";
	for ( const auto& it : elements ) {
		if ( !args.empty() ) {
			args += ", ";
		}
		args += it->ToString();
	}
	return "[ " + args + " ]";
}
const std::string Array::Dump( const size_t depth ) const {
	std::string result = Formatted( "Array" + m_si.ToString() + "( ", depth );
	for ( const auto& it : elements ) {
		result += Formatted( it->Dump(), depth + 1 );
	}
	return result + Formatted( ")", depth );
}

}
}
