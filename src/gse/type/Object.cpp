#include "Object.h"

#include "Null.h"

namespace gse {
namespace type {

static Value s_null = VALUE( type::Null );

Object::Object( properties_t initial_value )
	: Type( GetType() )
	, value( initial_value ) {}

const Value& Object::Get( const key_t& key ) const {
	const auto& it = value.find( key );
	return it == value.end()
		? s_null
		: it->second;
}

void Object::Set( const key_t& key, const Value& value ) {
	this->value.insert_or_assign( key, value );
}

}
}