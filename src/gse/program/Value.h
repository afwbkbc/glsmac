#pragma once

#include <string>

#include "Operand.h"

#include "gse/Value.h"

namespace gse {
namespace program {

class Value : public Operand {
public:

	Value( const si_t& si, gse::Value* const value );

	gse::Value* const value;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}

