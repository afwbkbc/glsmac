#pragma once

#include <string>

#include "Operand.h"

namespace gse {
namespace program {

class Variable : public Operand {
public:

	Variable( const std::string& name )
		: Operand( OT_VARIABLE )
		, name( name ) {}

	const std::string name;

	const std::string ToString( const size_t depth ) const override {
		return Formatted( "Variable( " + name + " )", depth );
	}
};

}
}

