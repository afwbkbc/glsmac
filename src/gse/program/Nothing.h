#pragma once

#include "Operand.h"

namespace gse {
namespace program {

class Nothing : public Operand {
public:

	Nothing( const si_t& si );

	const std::string Dump( const size_t depth = 0 ) const override;

};

}
}

