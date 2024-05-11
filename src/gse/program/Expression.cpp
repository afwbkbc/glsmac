#include "Expression.h"

#include "Operator.h"

namespace gse {
namespace program {

Expression::Expression( const si_t& si, const Operand* a, const Operator* op, const Operand* b )
	: Operand( si, OT_EXPRESSION )
	, a( a )
	, op( op )
	, b( b ) {}

Expression::~Expression() {
	if ( a ) {
		delete a;
	}
	if ( op ) {
		delete op;
	}
	if ( b ) {
		delete b;
	}
}

const std::string Expression::ToString() const {
	std::string result = "";
	if ( a ) {
		result += a->ToString();
	}
	if ( op ) {
		if ( !result.empty() ) {
			result += " ";
		}
		result += op->ToString();
	}
	if ( b ) {
		if ( !result.empty() ) {
			result += " ";
		}
		result += b->ToString();
	}
	return result;
}

const std::string Expression::Dump( const size_t depth ) const {
	return
		Formatted( "Expression" + m_si.ToString() + "(", depth ) +
			( a == nullptr
				? ""
				: a->Dump( depth + 1 )
			) +
			( op == nullptr
				? ""
				: op->Dump( depth + 1 )
			) +
			( b == nullptr
				? ""
				: b->Dump( depth + 1 )
			) +
			Formatted( ")", depth );
}

}
}
