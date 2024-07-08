#pragma once

#include "Conditional.h"

namespace gse {
namespace program {

class ForCondition;
class Scope;

class For : public Conditional {
public:

	For( const si_t& si, const ForCondition* condition, const Scope* body );
	~For();

	const ForCondition* condition;
	const Scope* body;

	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}
