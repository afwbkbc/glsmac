#pragma once

#include <vector>

#include "Operand.h"

#include "Variable.h"
#include "Scope.h"

namespace gse {
namespace program {

class Function : public Operand {
public:

	Function( const std::vector< Variable* >& parameters, const Scope* body )
		: Operand( OT_FUNCTION )
		, body( body ) {}

	const std::vector< Variable* > parameters;
	const Scope* body;

	~Function() {
		for ( auto& it : parameters ) {
			delete it;
		}
		delete body;
	}
};

}
}

