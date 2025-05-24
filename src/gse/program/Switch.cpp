#include "Switch.h"

#include "SimpleCondition.h"
#include "Case.h"

namespace gse {
namespace program {

Switch::Switch( const si_t& si, const SimpleCondition* condition, const std::vector< Case* >& cases )
	: Conditional( si, CT_SWITCH )
	, condition( condition )
	, cases( cases ) {}

Switch::~Switch() {
	delete condition;
	for ( const auto& c : cases ) {
		delete c;
	}
}

const std::string Switch::Dump( const size_t depth ) const {
	std::string result = Formatted( "Switch" + m_si.ToString() + "(", depth ) +
		Formatted( "Condition(", depth + 1 ) +
		condition->Dump( depth + 2 ) +
		Formatted( ")", depth + 1 ) +
		Formatted( "Body(", depth + 1 );
	for ( const auto& c : cases ) {
		result += c->Dump( depth + 3 );
	}
	result += Formatted( ")", depth + 1 ) +
		Formatted( ")", depth );
	return result;
}

}
}
