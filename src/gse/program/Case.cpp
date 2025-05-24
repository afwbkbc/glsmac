#include "Case.h"

#include "SimpleCondition.h"
#include "Scope.h"

namespace gse {
namespace program {

Case::Case( const si_t& si, const SimpleCondition* condition, const Scope* body )
	: Conditional( si, CT_CASE )
	, condition( condition )
	, body( body ) {}

Case::~Case() {
	delete condition;
	delete body;
}

const std::string Case::Dump( const size_t depth ) const {
	return Formatted( "Case" + m_si.ToString() + "(", depth ) +
		Formatted( "Condition(", depth + 1 ) +
		condition->Dump( depth + 2 ) +
		Formatted( ")", depth + 1 ) +
		Formatted( "Body(", depth + 1 ) +
		body->Dump( depth + 2 ) +
		Formatted( ")", depth + 1 ) +
		Formatted( ")", depth );
}

}
}
