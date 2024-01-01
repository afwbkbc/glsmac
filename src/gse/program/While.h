#pragma once

#include "Conditional.h"

#include "Expression.h"
#include "Scope.h"

namespace gse {
namespace program {

class While : public Conditional {
public:

	While( const si_t& si, const Expression* condition, const Scope* body )
		: Conditional( si, CT_WHILE )
		, condition( condition )
		, body( body ) {}

	const Expression* condition;
	const Scope* body;

	~While() {
		delete condition;
		delete body;
	}

	const std::string ToString( const size_t depth = 0 ) const override {
		return Formatted( "While" + m_si.ToString() + "(", depth ) +
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

