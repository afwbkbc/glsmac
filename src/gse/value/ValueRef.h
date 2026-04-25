#pragma once

#include "gse/Value.h"

namespace gse {
namespace value {

class Object;

class ValueRef : public Value {
public:

	static const value_type_t GetType() { return VT_VALUEREF; }

	ValueRef( gc::Space* const gc_space, Value* const target )
		: Value( gc_space, GetType() )
		, target( target ) {}

	Value* const target;

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

};

}
}
