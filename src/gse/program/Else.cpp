#include "Else.h"

#include "Scope.h"

namespace gse {
namespace program {

Else::Else( const si_t& si, const Scope* body )
	: Conditional( si, CT_ELSE )
	, body( body ) {}

Else::~Else() {
	delete body;
}

const std::string Else::Dump( const size_t depth ) const {
	return Formatted( "Else" + m_si.ToString() + "(", depth ) +
		Formatted( "Body(", depth + 1 ) +
		body->Dump( depth + 2 ) +
		Formatted( ")", depth + 1 ) +
		Formatted( ")", depth );
}

}
}
