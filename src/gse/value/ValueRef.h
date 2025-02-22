#pragma once

#include "gse/Value.h"

namespace gse {
namespace value {

class Object;

class ValueRef : public Value {
public:

	static const type_t GetType() { return Value::T_VALUEREF; }

	ValueRef( gc::Space* const gc_space, Value* const target )
		: Value( gc_space, GetType() )
		, target( target ) {}

	Value* const target;

	void GetReachableObjects( std::unordered_set< gc::Object* >& active_objects ) override;
};

}
}
