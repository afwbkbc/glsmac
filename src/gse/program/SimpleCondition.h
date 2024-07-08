#pragma once

#include "Condition.h"

namespace gse {
namespace program {

class Expression;

class SimpleCondition : public Condition {
public:
	SimpleCondition( const si_t& si, const Expression* expression );
	~SimpleCondition();

	const Expression* expression;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}
