#pragma once

#include <memory>

#include "type/Type.h"
#include "base/Base.h"

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

#define WRAPDEFS_CLASS() \
    static const gse::type::Object::object_class_t WRAP_CLASS;
#define WRAPDEFS_PTR( _type, ... ) \
    WRAPDEFS_CLASS() \
    virtual const gse::Value Wrap() __VA_ARGS__; \
    static _type* Unwrap( const gse::Value& value );
#define WRAPDEFS_DYNAMIC( _type ) \
    WRAPDEFS_CLASS() \
    const gse::Value Wrap( const bool dynamic = false ); \
    static _type* Unwrap( const gse::Value& value ); \
    static void WrapSet( void* wrapobj, const std::string& key, const gse::Value& value, gse::Context* ctx, const gse::si_t& si ); \
    void OnWrapSet( const std::string& property_name );
#define WRAPDEFS_NOPTR( _type ) \
    WRAPDEFS_CLASS() \
    const gse::Value Wrap(); \
    static _type Unwrap( const gse::Value& value );
#define WRAPIMPL_CLASS( _type, _class ) \
    const gse::type::Object::object_class_t _type::WRAP_CLASS = gse::type::Object::_class;
#define WRAPIMPL_BEGIN( _type, _class ) \
    WRAPIMPL_CLASS( _type, _class ) \
    const gse::Value _type::Wrap() {
#define WRAPIMPL_DYNAMIC_BEGIN( _type, _class ) \
    WRAPIMPL_CLASS( _type, _class ) \
    const gse::Value _type::Wrap( const bool dynamic ) {
#define WRAPIMPL_DYNAMIC_GETTERS( _type, _class ) \
    WRAPIMPL_DYNAMIC_BEGIN( _type, _class ) \
    const gse::type::Object::properties_t properties = {
#define WRAPIMPL_PROPS gse::type::Object::properties_t properties =
#define WRAPIMPL_PROPS_EXTEND( _parent ) \
    const auto wrapped_parent = _parent::Wrap(); \
    const auto& wrapped_parent_props = ( (gse::type::Object*)wrapped_parent.Get() )->value; \
    properties.insert( wrapped_parent_props.begin(), wrapped_parent_props.end() );
#define WRAPIMPL_END_PTR( _type ) \
    return VALUE( gse::type::Object, properties, WRAP_CLASS, this ); \
}
#define WRAPIMPL_DYNAMIC_SETTERS( _type ) \
    }; \
    return VALUE( gse::type::Object, properties, WRAP_CLASS, this, dynamic ? &_type::WrapSet : nullptr ); \
} \
void _type::WrapSet( void* wrapobj, const std::string& key, const gse::Value& value, gse::Context* ctx, const gse::si_t& si ) { \
    auto* obj = (_type*)wrapobj; \
    if ( false ) {}
#define WRAPIMPL_END_NOPTR( _type ) \
    return VALUE( gse::type::Object, properties, WRAP_CLASS ); \
}
#define WRAPIMPL_DYNAMIC_ON_SET( _type ) \
    else { \
        throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property does not exist", ctx, si ); \
    } \
} \
void _type::OnWrapSet( const std::string& property_name ) {
#define WRAPIMPL_DYNAMIC_END() \
}
#define WRAPIMPL_GET( _key, _type, _property ) \
    { \
        _key, \
        VALUE( gse::type::_type, _property ) \
    },
#define WRAPIMPL_LINK( _key, _property ) \
    { \
        _key, \
        NATIVE_CALL( this ) { \
            return _property->Wrap(); \
        }) \
    },

#define WRAPIMPL_SET( _key, _type, _property ) \
    else if ( key == _key ) { \
        if ( value.Get()->type != gse::type::_type::GetType() ) { \
            throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Invalid assignment value type, expected: " + gse::type::Type::GetTypeString( gse::type::_type::GetType() ) + ", got: " + gse::type::Type::GetTypeString( value.Get()->type ), ctx, si ); \
        } \
        obj->_property = ((gse::type::_type*)value.Get())->value; \
        obj->OnWrapSet( _key ); \
    }

#define UNWRAPIMPL_PTR( _type ) \
_type* _type::Unwrap( const gse::Value& value ) { \
    const auto* valueobj = value.Get()->Deref(); \
    ASSERT_NOLOG( valueobj->type == gse::type::Type::T_OBJECT, "can't unwrap non-object: " + valueobj->Dump() ); \
    const auto* obj = (gse::type::Object*)valueobj; \
    ASSERT_NOLOG( obj->object_class == WRAP_CLASS, "can't unwrap object of different class ( " + gse::type::Object::GetClassString( obj->object_class ) + " != " + gse::type::Object::GetClassString( WRAP_CLASS ) + " )" ); \
    ASSERT_NOLOG( obj->wrapobj, "can't unwrap object without internal link" ); \
    return (_type*)obj->wrapobj; \
}

#define UNWRAPIMPL_NOPTR_BEGIN( _type ) \
_type _type::Unwrap( const gse::Value& value ) { \
    const auto* valueobj = value.Get()->Deref(); \
    ASSERT_NOLOG( valueobj->type == gse::type::Type::T_OBJECT, "can only unwrap objects" ); \
    const auto* obj = (gse::type::Object*)valueobj; \
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

	static void Serialize( types::Buffer* buffer, const Value& value );
	static Value Unserialize( types::Buffer* buf );

private:
	std::shared_ptr< type::Type > m_data;

	const Value New( const type::Type* type ) const;
};

}
