#pragma once

#include "Conditional.h"

#include "Expression.h"
#include "Scope.h"

namespace gse {
namespace program {

class ElseIf : public Conditional {
public:

	ElseIf( const Expression* condition, const Scope* body, const Conditional* els = nullptr )
		: Conditional( CT_ELSEIF )
		, condition( condition )
		, body( body )
		, els( els ) {}

	const Expression* condition;
	const Scope* body;
	const Conditional* els;

	~ElseIf() {
		delete condition;
		delete body;
		if ( els ) {
			delete els;
		}
	}

	const std::string ToString( const size_t depth = 0 ) const override {
		return Formatted( "ElseIf(", depth ) +
			Formatted( "Condition(", depth + 1 ) +
			condition->ToString( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			Formatted( "Body(", depth + 1 ) +
			body->ToString( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			Formatted( ")", depth );
	}
};

}
}

