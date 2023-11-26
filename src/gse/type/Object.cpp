#include "Object.h"

#include "Null.h"

namespace gse {
namespace type {

static Value s_null = VALUE( Null );

const Value& Object::Get( const key_t& key ) const {
	const auto& it = m_properties.find( key );
	return it == m_properties.end()
		? s_null
		: it->second;
}

void Object::Set( const key_t& key, const Value& value ) {
	m_properties.insert_or_assign( key, value );
}

}
}