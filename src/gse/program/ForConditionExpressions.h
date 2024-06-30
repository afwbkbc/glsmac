#pragma once

#include "ForCondition.h"

namespace gse {
namespace program {

class Expression;

class ForConditionExpressions : public ForCondition {
public:
	ForConditionExpressions( const si_t& si, const Expression* init, const Expression* check, const Expression* iterate );
	~ForConditionExpressions();

	const Expression* init;
	const Expression* check;
	const Expression* iterate;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}
