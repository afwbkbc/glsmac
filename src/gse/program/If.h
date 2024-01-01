#pragma once

#include "Conditional.h"

#include "Expression.h"
#include "Scope.h"

namespace gse {
namespace program {

class If : public Conditional {
public:

	If( const si_t& si, const Expression* condition, const Scope* body, const Conditional* els = nullptr )
		: Conditional( si, CT_IF )
		, condition( condition )
		, body( body )
		, els( els ) {}

	const Expression* condition;
	const Scope* body;
	const Conditional* els;

	~If() {
		delete condition;
		delete body;
		if ( els ) {
			delete els;
		}
	}

	const std::string ToString( const size_t depth = 0 ) const override {
		return Formatted( "If" + m_si.ToString() + "(", depth ) +
			Formatted( "Condition(", depth + 1 ) +
			condition->ToString( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			Formatted( "Body(", depth + 1 ) +
			body->ToString( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			( els
				? els->ToString( depth + 1 )
				: ""
			) +
			Formatted( ")", depth );
	}
};

}
}

