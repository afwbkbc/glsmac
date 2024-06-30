#include "ForConditionInOf.h"

#include "Variable.h"
#include "Expression.h"

namespace gse {
namespace program {

ForConditionInOf::ForConditionInOf( const si_t& si, const Variable* variable, const for_inof_condition_type_t type, const Expression* expression )
	: ForCondition( si, FCT_IN_OF )
	, variable( variable )
	, for_inof_type( type )
	, expression( expression ) {}

ForConditionInOf::~ForConditionInOf() {
	if ( variable ) {
		delete variable;
	}
	if ( expression ) {
		delete expression;
	}
}

const std::string ForConditionInOf::ToString() const {
	return variable->ToString() + ( for_inof_type == FIC_IN
		? " in "
		: " of "
	) + expression->ToString();
}

const std::string ForConditionInOf::Dump( const size_t depth ) const {
	return
		Formatted( "ForCondition" + m_si.ToString() + "(", depth ) +
			variable->Dump( depth + 1 ) +
			Formatted(
				for_inof_type == FIC_IN
					? " in "
					: " of ",
				depth + 1
			) +
			expression->Dump( depth + 1 )
			+
				Formatted( ")", depth );
}

}
}
