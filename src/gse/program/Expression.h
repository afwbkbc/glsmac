#pragma once

#include "Operand.h"

namespace gse {
namespace program {

class Operator;

class Expression : public Operand {
public:

	Expression( const si_t& si, const Operand* a, const Operator* op = nullptr, const Operand* b = nullptr );
	~Expression();

	const Operand* a;
	const Operator* op;
	const Operand* b;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}
