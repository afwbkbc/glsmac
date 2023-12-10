#pragma once

#include <vector>

#include "Operand.h"

#include "Expression.h"

namespace gse {
namespace program {

class Call : public Operand {
public:

	Call( const Expression* callable, const std::vector< Operand* >& arguments )
		: Operand( OT_CALL )
		, callable( callable )
		, arguments( arguments ) {}

	const Expression* callable;
	const std::vector< Operand* > arguments;

	~Call() {
		delete callable;
		for ( auto& it : arguments ) {
			delete it;
		}
	}

};

}
}

