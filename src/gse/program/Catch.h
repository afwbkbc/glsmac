#pragma once

#include "Conditional.h"

#include "Object.h"

namespace gse {
namespace program {

class Catch : public Conditional {
public:

	Catch( const si_t& si, const Object* handlers )
		: Conditional( si, CT_CATCH )
		, handlers( handlers ) {}

	const Object* handlers;

	~Catch() {
		delete handlers;
	}

	const std::string Dump( const size_t depth = 0 ) const override {
		return Formatted( "Catch" + m_si.ToString() + "(", depth ) +
			Formatted( "Handlers(", depth + 1 ) +
			handlers->Dump( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			Formatted( ")", depth );
	}
};

}
}

