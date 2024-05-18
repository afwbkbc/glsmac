#pragma once

#include <vector>

#include "Operand.h"

namespace gse {
namespace program {

class Variable;
class Scope;

class Function : public Operand {
public:

	Function( const si_t& si, const std::vector< Variable* >& parameters, const Scope* body );
	~Function();

	const std::vector< Variable* > parameters;
	const Scope* body;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}

