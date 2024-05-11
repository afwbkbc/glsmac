#include "If.h"

#include "Expression.h"
#include "Scope.h"

namespace gse {
namespace program {

If::If( const si_t& si, const Expression* condition, const Scope* body, const Conditional* els )
	: Conditional( si, CT_IF )
	, condition( condition )
	, body( body )
	, els( els ) {}

If::~If() {
	delete condition;
	delete body;
	if ( els ) {
		delete els;
	}
}

const std::string If::Dump( const size_t depth ) const {
	return Formatted( "If" + m_si.ToString() + "(", depth ) +
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
