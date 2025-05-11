#pragma once

#include <optional>

#include "gse/Value.h"

namespace gse {
namespace value {

class Range : public Value {
public:

	static const type_t GetType() { return Value::T_RANGE; }

	Range( gc::Space* const gc_space, const std::optional< size_t > from, const std::optional< size_t > to )
		: Value( gc_space, GetType() )
		, from( from )
		, to( to ) {}

	const std::optional< size_t > from;
	const std::optional< size_t > to;

};

}
}
