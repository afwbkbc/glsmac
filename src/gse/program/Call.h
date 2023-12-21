#pragma once

#include <vector>

#include "Expression.h"

namespace gse {
namespace program {

class Call : public Operand {
public:

	Call( const Expression* callable, const std::vector< const Expression* >& arguments )
		: Operand( OT_CALL )
		, callable( callable )
		, arguments( arguments ) {}

	const Expression* callable;
	const std::vector< const Expression* > arguments;

	~Call() {
		delete callable;
		for ( auto& it : arguments ) {
			delete it;
		}
	}

	const std::string ToString( const size_t depth ) const override {
		std::string args = "";
		for ( const auto& it : arguments ) {
			args += it->ToString( depth + 2 );
		}
		return Formatted( "Call(", depth ) +
			Formatted( "Callable(", depth + 1 ) +
			callable->ToString( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			Formatted( "Arguments(", depth + 1 ) +
			args +
			Formatted( ")", depth + 1 ) +
			Formatted( ")", depth );

	}
};

}
}

