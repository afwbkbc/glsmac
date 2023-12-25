#pragma once

#include "type/Type.h"
#include "base/Base.h"

#include <memory>

namespace gse {

#define VALUE( _type, ... ) gse::Value( std::make_shared<_type>( __VA_ARGS__ ) )
#ifdef DEBUG
#define VALUE_DATA( _type, _var ) ( _var.Get()->type == _type::GetType() ? ((_type*)_var.Get()) : THROW( "invalid GSE value type (expected " + type::Type::GetTypeString( _type::GetType() ) + ", got " + type::Type::GetTypeString( _var.Get()->type ) + ")" ) )
#else
#define VALUE_DATA( _type, _var ) ((_type*)_var.Get())
#endif
#define VALUE_GET( _type, _var ) VALUE_DATA( _type, _var )->value
#define VALUE_SET( _type, _var, _value ) { VALUE_DATA( _type, _var )->value = _value; }
#define VALUE_CLONE( _type, _var ) VALUE( _type, VALUE_GET( _type, _var ) )

class Value {
public:
	Value() = delete;
	Value( const std::shared_ptr< type::Type > data );
	~Value();

	const type::Type* Get() const;
	const std::string ToString() const;

#define OP( _op ) const bool operator _op( const Value& other ) const;
	OP( == )
	OP( != )
	OP( < )
	OP( > )
	OP( <= )
	OP( >= )
#undef OP

private:
	std::shared_ptr< type::Type > m_data;
};

}
