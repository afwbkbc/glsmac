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
};

}
}

