#include "ElseIf.h"

#include "SimpleCondition.h"
#include "Scope.h"

namespace gse {
namespace program {

ElseIf::ElseIf( const si_t& si, const SimpleCondition* condition, const Scope* body, const Conditional* els )
	: Conditional( si, CT_ELSEIF )
	, condition( condition )
	, body( body )
	, els( els ) {}

ElseIf::~ElseIf() {
	delete condition;
	delete body;
	if ( els ) {
		delete els;
	}
}

const std::string ElseIf::Dump( const size_t depth ) const {
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

}
}
