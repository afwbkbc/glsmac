#pragma once

#include <string>

#include "Operand.h"

#include "gse/Value.h"

namespace gse {
namespace program {

class Value : public Operand {
public:

	Value( const gse::Value& value )
		: Operand( OT_VALUE )
		, value( value ) {}

	const gse::Value value;

	const std::string ToString( const size_t depth = 0 ) const override {
		return Formatted( "Value( " + value.ToString() + " )", depth );
	}
};

}
}

