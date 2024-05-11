#include "Catch.h"

#include "Object.h"

namespace gse {
namespace program {

Catch::Catch( const si_t& si, const Object* handlers )
	: Conditional( si, CT_CATCH )
	, handlers( handlers ) {}

Catch::~Catch() {
	delete handlers;
}

const std::string Catch::Dump( const size_t depth ) const {
	return Formatted( "Catch" + m_si.ToString() + "(", depth ) +
		Formatted( "Handlers(", depth + 1 ) +
		handlers->Dump( depth + 2 ) +
		Formatted( ")", depth + 1 ) +
		Formatted( ")", depth );
}

}
}
