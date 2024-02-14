#pragma once

#include <map>
#include <string>

#include "Type.h"
#include "../Value.h"

namespace gse {
namespace type {

static const std::string OBJECT_CLASS_TILE = "Tile";

class Object : public Type {
public:

	// internal use only for now
	enum object_class_t {
		CLASS_NONE,
		CLASS_EXCEPTION,
		CLASS_TILE,
		CLASS_UNIT,
		CLASS_UNITDEF,
	};
	static const std::string& GetClassString( const object_class_t object_class );

	static const type_t GetType() { return Type::T_OBJECT; }

	typedef std::string key_t; // keep it simple for now
	typedef std::map< key_t, Value > properties_t;

	Object( properties_t initial_value = {}, const object_class_t object_class = CLASS_NONE );

	const Value& Get( const key_t& key ) const;
	void Set( const key_t& key, const Value& value );

	const Value GetRef( const key_t& key );

	properties_t value = {};
	const object_class_t object_class;

};

}
}
