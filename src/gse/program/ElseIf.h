#pragma once

#include "Conditional.h"

#include "Expression.h"
#include "Scope.h"

namespace gse {
namespace program {

class ElseIf : public Conditional {
public:

	ElseIf( const si_t& si, const Expression* condition, const Scope* body, const Conditional* els = nullptr )
		: Conditional( si, CT_ELSEIF )
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

	const std::string Dump( const size_t depth = 0 ) const override {
		return Formatted( "ElseIf" + m_si.ToString() + "(", depth ) +
			Formatted( "Condition(", depth + 1 ) +
			condition->Dump( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			Formatted( "Body(", depth + 1 ) +
			body->Dump( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			( els
				? els->Dump( depth + 1 )
				: ""
			) +
			Formatted( ")", depth );
	}
};

}
}

