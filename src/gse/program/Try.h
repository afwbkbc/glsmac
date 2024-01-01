#pragma once

#include "Conditional.h"

#include "Scope.h"
#include "Catch.h"

namespace gse {
namespace program {

class Try : public Conditional {
public:

	Try( const si_t& si, const Scope* body, const Catch* handlers )
		: Conditional( si, CT_TRY )
		, body( body )
		, handlers( handlers ) {}

	const Scope* body;
	const Catch* handlers;

	~Try() {
		delete body;
	}

	const std::string ToString( const size_t depth = 0 ) const override {
		return Formatted( "Try(", depth ) +
			Formatted( "Body(", depth + 1 ) +
			body->ToString( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			Formatted( ")", depth );
	}
};

}
}

