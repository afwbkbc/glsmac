#pragma once

#include <vector>

#include "Operand.h"
#include "Expression.h"

namespace gse {
namespace program {

class Array : public Operand {
public:
	typedef std::vector< const Expression* > elements_t;

	Array( const elements_t& elements )
		: Operand( OT_ARRAY )
		, elements( elements ) {}

	const elements_t elements;

	~Array() {
		for ( auto& it : elements ) {
			delete it;
		}
	}

	const std::string ToString( const size_t depth = 0 ) const override {
		std::string result = Formatted( "Array( ", depth );
		for ( const auto& it : elements ) {
			result += Formatted( it->ToString(), depth + 1 );
		}
		return result + Formatted( ")", depth );
	}
};

}
}

