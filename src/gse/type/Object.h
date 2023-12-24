#pragma once

#include <map>
#include <string>

#include "Type.h"
#include "../Value.h"

namespace gse {
namespace type {

class Object : public Type {
public:

	static const type_t GetType() { return Type::T_OBJECT; }

	typedef std::string key_t; // keep it simple for now
	typedef std::map< key_t, Value > properties_t;

	Object( properties_t initial_value = {} );

	const Value& Get( const key_t& key ) const;
	void Set( const key_t& key, const Value& value );

	const Value GetRef( const key_t& key );

	properties_t value = {};
};

}
}