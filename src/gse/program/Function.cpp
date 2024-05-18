#include "Function.h"

#include "Variable.h"
#include "Scope.h"

namespace gse {
namespace program {

Function::Function( const si_t& si, const std::vector< Variable* >& parameters, const Scope* body )
	: Operand( si, OT_FUNCTION )
	, parameters( parameters )
	, body( body ) {}

Function::~Function() {
	for ( auto& it : parameters ) {
		delete it;
	}
	delete body;
}

const std::string Function::ToString() const {
	std::string params = "";
	for ( const auto& it : parameters ) {
		if ( !params.empty() ) {
			params += ", ";
		}
		params += it->ToString();
	}
	return "( " + params + " ) => " + body->ToString();
}
const std::string Function::Dump( const size_t depth ) const {
	std::string params = "";
	for ( const auto& it : parameters ) {
		params += it->Dump( depth + 2 );
	}
	return Formatted( "Function" + m_si.ToString() + "(", depth ) +
		Formatted( "Parameters(", depth + 1 ) +
		params +
		Formatted( ")", depth + 1 ) +
		Formatted( "Body(", depth + 1 ) +
		body->Dump( depth + 2 ) +
		Formatted( ")", depth + 1 ) +
		Formatted( ")", depth );
}

}
}
