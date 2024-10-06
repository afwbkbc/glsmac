#include "SimpleCondition.h"

#include "Expression.h"

namespace gse {
namespace program {

SimpleCondition::SimpleCondition( const si_t& si, const Expression* expression )
	: Condition( si, CT_SIMPLE )
	, expression( expression ) {}

SimpleCondition::~SimpleCondition() {
	if ( expression ) {
		delete expression;
	}
}

const std::string SimpleCondition::ToString() const {
	std::string result = "";
	if ( expression ) {
		result += expression->ToString();
	}
	return result;
}

const std::string SimpleCondition::Dump( const size_t depth ) const {
	return
		Formatted( "SimpleCondition" + m_si.ToString() + "(", depth ) +
			( expression == nullptr
				? ""
				: expression->Dump( depth + 1 )
			) +
			Formatted( ")", depth );
}

}
}
