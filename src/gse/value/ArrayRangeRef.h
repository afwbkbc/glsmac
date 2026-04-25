#pragma once

#include "gse/Value.h"

#include <optional>

namespace gse {
namespace value {

class Array;

class ArrayRangeRef : public Value {
public:

	static const value_type_t GetType() { return VT_ARRAYRANGEREF; }

	ArrayRangeRef( gc::Space* const gc_space, Array* array, const std::optional< size_t > from, const std::optional< size_t > to )
		: Value( gc_space, GetType() )
		, array( array )
		, from( from )
		, to( to ) {}

	Array* array;
	const std::optional< size_t > from;
	const std::optional< size_t > to;

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

};

}
}
