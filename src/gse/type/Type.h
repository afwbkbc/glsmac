#pragma once

#include <cstdint>
#include <string>

namespace gse {
namespace type {

class Type {
public:
	enum type_t : uint8_t {
		T_UNDEFINED,
		T_NULL,
		T_BOOL,
		T_INT,
		T_FLOAT,
		T_STRING,
		T_TUPLE,
		T_OBJECT,
		T_CALLABLE,
	};

	static const std::string GetTypeString( const type_t type );
	const std::string ToString() const;
	const bool operator==( const Type& other ) const;

	const type_t type;

protected:
	Type( const type_t type )
		: type( type ) {}

};

}
}
