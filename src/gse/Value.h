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

#define WRAPDEFS_PTR( _type ) \
    static const gse::type::Object::object_class_t WRAP_CLASS; \
    const gse::Value Wrap() const; \
    static _type* Unwrap( const gse::Value& value );
#define WRAPDEFS_NOPTR( _type ) \
    static const gse::type::Object::object_class_t WRAP_CLASS; \
    const gse::Value Wrap() const; \
    static _type Unwrap( const gse::Value& value );
#define WRAPIMPL_BEGIN( _type, _class ) \
const gse::type::Object::object_class_t _type::WRAP_CLASS = gse::type::Object::_class; \
const gse::Value _type::Wrap() const {
#define WRAPIMPL_PROPS const gse::type::Object::properties_t properties =
#define WRAPIMPL_END_PTR( _type ) \
    return VALUE( gse::type::Object, properties, WRAP_CLASS, this ); \
}
#define WRAPIMPL_END_NOPTR( _type ) \
    return VALUE( gse::type::Object, properties, WRAP_CLASS ); \
}

#define UNWRAPIMPL_PTR( _type ) \
_type* _type::Unwrap( const gse::Value& value ) { \
    ASSERT_NOLOG( value.Get()->type == gse::type::Type::T_OBJECT, "can't unwrap non-object: " + value.Dump() ); \
    const auto* obj = (gse::type::Object*)value.Get(); \
    ASSERT_NOLOG( obj->object_class == WRAP_CLASS, "can't unwrap object of different class ( " + gse::type::Object::GetClassString( obj->object_class ) + " != " + gse::type::Object::GetClassString( WRAP_CLASS ) + " )" ); \
    ASSERT_NOLOG( obj->wrapptr, "can't unwrap object without internal link" ); \
    return (_type*)obj->wrapptr; \
}

#define UNWRAPIMPL_NOPTR_BEGIN( _type ) \
_type _type::Unwrap( const gse::Value& value ) { \
    ASSERT_NOLOG( value.Get()->type == gse::type::Type::T_OBJECT, "can only unwrap objects" ); \
    const auto* obj = (gse::type::Object*)value.Get(); \
    ASSERT_NOLOG( obj->object_class == WRAP_CLASS, "can only unwrap objects of same class" ); \
    const auto& properties = obj->value;

#define UNWRAPIMPL_NOPTR_END() \
}

class Value {
public:
	Value() = delete;
	Value( const std::shared_ptr< type::Type > data );
	~Value();

	const type::Type* Get() const;
	const std::string& GetTypeString() const;
	const std::string ToString() const;
	const std::string Dump() const;

	const Value Clone() const;

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
