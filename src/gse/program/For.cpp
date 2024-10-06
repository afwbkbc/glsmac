#include "For.h"

#include "ForCondition.h"
#include "Scope.h"

namespace gse {
namespace program {

For::For( const si_t& si, const ForCondition* condition, const Scope* body )
	: Conditional( si, CT_FOR )
	, condition( condition )
	, body( body ) {}

For::~For() {
	delete condition;
	delete body;
}

const std::string For::Dump( const size_t depth ) const {
	return Formatted( "For" + m_si.ToString() + "(", depth ) +
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
