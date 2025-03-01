#pragma once

#include "gse/Value.h"

namespace gse {
namespace value {

class Object;

class ObjectRef : public Value {
public:

	static const type_t GetType() { return Value::T_OBJECTREF; }

	ObjectRef( gc::Space* const gc_space, value::Object* object, const std::string& key )
		: Value( gc_space, GetType() )
		, object( object )
		, key( key ) {}

	value::Object* object;
	const std::string key;

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;
};

}
}
