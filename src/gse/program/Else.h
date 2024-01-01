#pragma once

#include "Conditional.h"

#include "Expression.h"
#include "Scope.h"

namespace gse {
namespace program {

class Else : public Conditional {
public:

	Else( const si_t& si, const Scope* body )
		: Conditional( si, CT_ELSE )
		, body( body ) {}

	const Scope* body;

	~Else() {
		delete body;
	}

	const std::string ToString( const size_t depth = 0 ) const override {
		return Formatted( "Else(", depth ) +
			Formatted( "Body(", depth + 1 ) +
			body->ToString( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			Formatted( ")", depth );
	}
};

}
}

