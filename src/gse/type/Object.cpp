#include "Object.h"

#include "Null.h"

namespace gse {
namespace type {

static Null s_null = Null();

const Type& Object::Get( const key_t& key ) const {
	const auto& it = m_properties.find( key );
	return it == m_properties.end()
		? s_null
		: it->second;
}

void Object::Set( const key_t& key, const Type& value ) {
	m_properties[ key ] = value;
}

}
}