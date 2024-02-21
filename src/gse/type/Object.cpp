#include "Object.h"

#include <unordered_map>

#include "Undefined.h"
#include "ObjectRef.h"

#include "String.h"

namespace gse {
namespace type {

static Value s_undefined = VALUE( type::Undefined );

static const std::unordered_map< Object::object_class_t, std::string > s_object_class_str = {
	{
		Object::CLASS_NONE,
		""
	},
	{
		Object::CLASS_EXCEPTION,
		"#exception"
	},
	{
		Object::CLASS_COLOR,
		"#color"
	},
	{
		Object::CLASS_TILE,
		"#tile"
	},
	{
		Object::CLASS_PLAYER,
		"#player"
	},
	{
		Object::CLASS_FACTION,
		"#faction"
	},
	{
		Object::CLASS_UNITDEF,
		"#unitdef"
	},
	{
		Object::CLASS_UNIT,
		"#unit"
	},
};
const std::string& Object::GetClassString( const object_class_t object_class ) {
	const auto& it = s_object_class_str.find( object_class );
	ASSERT_NOLOG( it != s_object_class_str.end(), "unknown/unsupported object class type: " + std::to_string( object_class ) );
	return it->second;
}

Object::Object( properties_t initial_value, const object_class_t object_class, const void* wrapptr )
	: Type( GetType() )
	, value( initial_value )
	, object_class( object_class )
	, wrapptr( wrapptr ) {}

const Value& Object::Get( const key_t& key ) const {
	const auto& it = value.find( key );
	return it == value.end()
		? s_undefined
		: it->second;
}

void Object::Set( const key_t& key, const Value& new_value ) {
	value.insert_or_assign( key, new_value );
}

const Value Object::GetRef( const key_t& key ) {
	return VALUE( ObjectRef, this, key );
}

}
}
