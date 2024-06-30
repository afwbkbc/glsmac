#pragma once

#include <cstdint>
#include <string>

namespace types {
class Buffer;
}

namespace gse {
class Value;
namespace type {

class Type {
public:
	enum type_t : uint8_t {
		T_NOTHING,
		T_UNDEFINED,
		T_NULL,
		T_BOOL,
		T_INT,
		T_FLOAT,
		T_STRING,
		T_ARRAY,
		T_OBJECT,
		T_CALLABLE,
		T_ARRAYREF,
		T_ARRAYRANGEREF,
		T_OBJECTREF,
		T_RANGE,
		T_LOOPCONTROL,
	};

	static const std::string& GetTypeString( const type_t type );
	const std::string ToString() const;
	const std::string Dump() const;
	const Type* Deref() const;

#define OP( _op ) const bool operator _op( const Type& other ) const;
	OP( == )
	OP( != )
	OP( < )
	OP( > )
	OP( <= )
	OP( >= )
#undef OP

	const type_t type;

protected:
	Type( const type_t type )
		: type( type ) {}

private:
	friend class gse::Value;

	static void Serialize( types::Buffer* buf, const Type* type );
	static Value Unserialize( types::Buffer* buf );

};

}
}

