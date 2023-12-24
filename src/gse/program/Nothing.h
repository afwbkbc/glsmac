#pragma once

#include <string>

#include "Operand.h"

namespace gse {
namespace program {

class Nothing : public Operand {
public:

	Nothing()
		: Operand( OT_NOTHING ) {}

	const std::string ToString( const size_t depth = 0 ) const override {
		return Formatted( "Nothing()", depth );
	}

};

}
}

