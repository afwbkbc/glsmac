#pragma once

#include "Element.h"

#include "Operand.h"
#include "Operator.h"
#include "Nothing.h"

namespace gse {
namespace program {

class Expression : public Operand {
public:
	Expression( const Operand* a, const Operator* op = new Operator( Operator::OT_NOOP ), const Operand* b = new Nothing() )
		: Operand( OT_EXPRESSION )
		, a( a )
		, op( op )
		, b( b ) {}

	const Operand* a;
	const Operator* op;
	const Operand* b;

	~Expression() {
		delete a;
		delete op;
		delete b;
	}
};

}
}
