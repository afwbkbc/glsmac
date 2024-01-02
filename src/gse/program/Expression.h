#pragma once

#include "Element.h"

#include "Operand.h"
#include "Operator.h"
#include "Nothing.h"

namespace gse {
namespace program {

class Expression : public Operand {
public:
	Expression( const si_t& si, const Operand* a, const Operator* op = nullptr, const Operand* b = nullptr )
		: Operand( si, OT_EXPRESSION )
		, a( a )
		, op( op )
		, b( b ) {}

	const Operand* a;
	const Operator* op;
	const Operand* b;

	~Expression() {
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

	const std::string ToString( const size_t depth = 0 ) const override {
		return
			Formatted( "Expression" + m_si.ToString() + "(", depth ) +
				( a == nullptr
					? ""
					: a->ToString( depth + 1 )
				) +
				( op == nullptr
					? ""
					: op->ToString( depth + 1 )
				) +
				( b == nullptr
					? ""
					: b->ToString( depth + 1 )
				) +
				Formatted( ")", depth );
	}
};

}
}
