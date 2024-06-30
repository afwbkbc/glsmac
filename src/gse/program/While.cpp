#include "While.h"

#include "SimpleCondition.h"
#include "Scope.h"

namespace gse {
namespace program {

While::While( const si_t& si, const SimpleCondition* condition, const Scope* body )
	: Conditional( si, CT_WHILE )
	, condition( condition )
	, body( body ) {}

While::~While() {
	delete condition;
	delete body;
}

const std::string While::Dump( const size_t depth ) const {
	return Formatted( "While" + m_si.ToString() + "(", depth ) +
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
