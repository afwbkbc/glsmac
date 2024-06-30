#pragma once

#include <vector>

#include "Operand.h"

namespace gse {
namespace program {

class Expression;

class Call : public Operand {
public:

	Call( const si_t& si, const Expression* callable, const std::vector< const Expression* >& arguments );
	~Call();

	const Expression* callable;
	const std::vector< const Expression* > arguments;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}
