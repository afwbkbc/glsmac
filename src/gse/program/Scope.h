#pragma once

#include <vector>

#include "Operand.h"

namespace gse {
namespace program {

class Control;

class Scope : public Operand {
public:

	Scope( const si_t& si, const std::vector< const Control* >& body );
	~Scope();

	const std::vector< const Control* > body;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}
