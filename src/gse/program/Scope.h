#pragma once

#include <vector>

#include "Operand.h"
#include "Control.h"

namespace gse {
namespace program {

class Scope : public Operand {
public:

	Scope( const std::vector< const Control* >& body )
		: Operand( OT_SCOPE )
		, body( body ) {}

	const std::vector< const Control* > body;

	~Scope() {
		for ( auto& it : body ) {
			delete it;
		}
	}

	const std::string ToString( const size_t depth = 0 ) const override {
		std::string result = Formatted( "Scope(", depth );
		for ( const auto& it : body ) {
			result += it->ToString( depth + 1 );
		}
		return result + Formatted( ")", depth );
	}
};

}
}
