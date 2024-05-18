#pragma once

#include <vector>

#include "Operand.h"

namespace gse {
namespace program {

class Expression;

class Array : public Operand {
public:
	typedef std::vector< const Expression* > elements_t;

	Array( const si_t& si, const elements_t& elements );
	~Array();

	const elements_t elements;

	const std::string ToString() const;
	const std::string Dump( const size_t depth = 0 ) const;
};

}
}
