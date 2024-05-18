#pragma once

#include "Conditional.h"

namespace gse {
namespace program {

class Scope;

class Else : public Conditional {
public:

	Else( const si_t& si, const Scope* body );
	~Else();

	const Scope* body;

	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}

