#pragma once

#include <string>

#include "Operand.h"

#include "Types.h"

namespace gse {
namespace program {

class Variable : public Operand {
public:

	Variable( const si_t& si, const std::string& name, const variable_hints_t hints = VH_NONE );

	const std::string name;
	const variable_hints_t hints;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}

