#pragma once

#include "ForCondition.h"

namespace gse {
namespace program {

class Variable;
class Expression;

class ForConditionInOf : public ForCondition {
public:
	enum for_inof_condition_type_t {
		FIC_NOTHING,
		FIC_IN,
		FIC_OF,
	};

	ForConditionInOf( const si_t& si, const Variable* variable, const for_inof_condition_type_t type, const Expression* expression );
	~ForConditionInOf();

	const Variable* variable;
	const for_inof_condition_type_t for_inof_type;
	const Expression* expression;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}
