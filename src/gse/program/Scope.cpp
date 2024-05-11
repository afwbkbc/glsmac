#include "Scope.h"

#include "Control.h"

namespace gse {
namespace program {

Scope::Scope( const si_t& si, const std::vector< const Control* >& body )
	: Operand( si, OT_SCOPE )
	, body( body ) {}

Scope::~Scope() {
	for ( auto& it : body ) {
		delete it;
	}
}

const std::string Scope::ToString() const {
	return "{ ... }";
}
const std::string Scope::Dump( const size_t depth ) const {
	std::string result = Formatted( "Scope" + m_si.ToString() + "(", depth );
	for ( const auto& it : body ) {
		result += it->Dump( depth + 1 );
	}
	return result + Formatted( ")", depth );
}

}
}
