#pragma once

#include "gse/Value.h"

namespace gse {
namespace value {

class Object;

class ObjectRef : public Value {
public:

	static const type_t GetType() { return Value::T_OBJECTREF; }

	ObjectRef( Object* object, const std::string& key )
		: Value( GetType() )
		, object( object )
		, key( key ) {}

	Object* object;
	const std::string key;

};

}
}
