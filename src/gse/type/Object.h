#pragma once

#include <map>
#include <string>

#include "Type.h"

namespace gse {
namespace type {

class Object : public Type {
public:
	typedef std::string key_t; // keep it simple for now

	Object( const std::string& class_name )
		: Type( Type::T_OBJECT )
		, class_name( class_name ) {}

	const std::string class_name;

	Object( const Object& other )
		: Object( other.class_name ) {
		m_properties = other.m_properties;
	}

	const Type& Get( const key_t& key ) const;

	void Set( const key_t& key, const Type& value );

private:
	std::map< key_t, Type > m_properties = {};
};

}
}