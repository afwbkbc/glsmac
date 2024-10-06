#pragma once

#include "Conditional.h"

namespace gse {
namespace program {

class SimpleCondition;
class Scope;

class While : public Conditional {
public:

	While( const si_t& si, const SimpleCondition* condition, const Scope* body );
	~While();

	const SimpleCondition* condition;
	const Scope* body;

	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}

