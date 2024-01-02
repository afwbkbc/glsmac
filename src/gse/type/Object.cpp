#include "Object.h"
#include "Undefined.h"
#include "ObjectRef.h"

#include "String.h"

namespace gse {
namespace type {

static Value s_undefined = VALUE( type::Undefined );

Object::Object( properties_t initial_value )
	: Type( GetType() )
	, value( initial_value ) {}

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
