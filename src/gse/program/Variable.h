#pragma once

#include <string>

#include "Operand.h"

namespace gse {
namespace program {

class Variable : public Operand {
public:

	enum variable_hints_t : uint8_t {
		VH_NONE,
		VH_CREATE,
	};

	Variable( const si_t& si, const std::string& name, const variable_hints_t hints = VH_NONE )
		: Operand( si, OT_VARIABLE )
		, name( name )
		, hints( hints ) {}

	const std::string name;
	const variable_hints_t hints;

	const std::string ToString( const size_t depth = 0 ) const override {
		return Formatted( "Variable" + m_si.ToString() + "( " + name + " )", depth );
	}
};

}
}

