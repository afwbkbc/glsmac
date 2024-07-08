#pragma once

#include <string>

#include "Operand.h"

#include "Types.h"

namespace gse {
namespace program {

class LoopControl : public Operand {
public:

	LoopControl( const si_t& si, const loop_control_type_t loop_control_type );

	const loop_control_type_t loop_control_type;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}
