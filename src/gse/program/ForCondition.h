#pragma once

#include "Condition.h"

namespace gse {
namespace program {

class ForCondition : public Condition {
public:
	enum for_condition_type_t {
		FCT_NOTHING,
		FCT_IN_OF,
		FCT_EXPRESSIONS,
	};

	ForCondition( const si_t& si, const for_condition_type_t type );
	virtual ~ForCondition() = default;

	const for_condition_type_t for_type;

	virtual const std::string ToString() const = 0;
	virtual const std::string Dump( const size_t depth = 0 ) const = 0;
};

}
}
