#include "ForConditionExpressions.h"

#include "Variable.h"
#include "Expression.h"

namespace gse {
namespace program {

ForConditionExpressions::ForConditionExpressions( const si_t& si, const Expression* init, const Expression* check, const Expression* iterate )
	: ForCondition( si, FCT_EXPRESSIONS )
	, init( init )
	, check( check )
	, iterate( iterate ) {}

ForConditionExpressions::~ForConditionExpressions() {
	if ( init ) {
		delete init;
	}
	if ( iterate ) {
		delete iterate;
	}
	if ( check ) {
		delete check;
	}
}

const std::string ForConditionExpressions::ToString() const {
	return init->ToString() + " ; " + iterate->ToString() + " ; " + check->ToString();
}

const std::string ForConditionExpressions::Dump( const size_t depth ) const {
	return
		Formatted( "ForCondition" + m_si.ToString() + "(", depth ) +
			init->Dump( depth + 1 ) +
			Formatted( ";", depth + 1 ) +
			iterate->Dump( depth + 1 ) +
			Formatted( ";", depth + 1 ) +
			check->Dump( depth + 1 ) +
			Formatted( ")", depth );
}

}
}
