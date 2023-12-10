#pragma once

#include <vector>

#include "Operand.h"
#include "Statement.h"

namespace gse {
namespace program {

class Scope : public Operand {
public:

	Scope( const std::vector< Statement* >& body )
		: Operand( OT_SCOPE )
		, body( body ) {}

	const std::vector< Statement* > body;

	~Scope() {
		for ( auto& it : body ) {
			delete it;
		}
	}
};

}
}
