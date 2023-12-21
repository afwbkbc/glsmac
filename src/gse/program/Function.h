#pragma once

#include <vector>

#include "Operand.h"

#include "Variable.h"
#include "Scope.h"

namespace gse {
namespace program {

class Function : public Operand {
public:

	Function( const std::vector< Variable* >& parameters, const Scope* body )
		: Operand( OT_FUNCTION )
		, parameters( parameters )
		, body( body ) {}

	const std::vector< Variable* > parameters;
	const Scope* body;

	~Function() {
		for ( auto& it : parameters ) {
			delete it;
		}
		delete body;
	}

	const std::string ToString( const size_t depth ) const override {
		std::string params = "";
		for ( const auto& it : parameters ) {
			params += it->ToString( depth + 2 );
		}
		return Formatted( "Function(", depth ) +
			Formatted( "Parameters(", depth + 1 ) +
			params +
			Formatted( ")", depth + 1 ) +
			Formatted( "Body(", depth + 1 ) +
			body->ToString( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			Formatted( ")", depth );
	}
};

}
}

