#pragma once

#include <memory>

#include "common/Assert.h"

namespace types {
class Buffer;
}

namespace gse {

#define GSE_ERROR( _type, _text ) throw gse::Exception( _type, _text, GSE_CALL );

#define GSE_CALLABLE gse::context::Context* ctx, const gse::si_t& si, gse::ExecutionPointer& ep
#define GSE_CALL ctx, si, ep

#define VALUE( _type, ... ) ( new _type( __VA_ARGS__ ) )
#ifdef DEBUG
#define VALUE_DATA( _type, _var ) ( _var->type == _type::GetType() ? ((_type*)_var) : THROW( "invalid GSE value type (expected " + Value::GetTypeStringStatic( _type::GetType() ) + ", got " + _var->GetTypeString() + ")" ) )
#else
#define VALUE_DATA( _type, _var ) ((_type*)_var)
#endif
#define VALUE_GET( _type, _var ) VALUE_DATA( _type, _var )->value
#define VALUE_SET( _type, _var, _value ) { VALUE_DATA( _type, _var )->value = _value; }
#define VALUE_CLONE( _type, _var ) VALUE( _type, VALUE_GET( _type, _var ) )

#define WRAPDEFS_CLASS() \
    static const gse::value::Object::object_class_t WRAP_CLASS;
#define WRAPDEFS_PTR( _type ) \
    WRAPDEFS_CLASS() \
    virtual gse::Value* const Wrap( const bool dynamic = false ) override; \
    static _type* Unwrap( gse::Value* const value );
#define WRAPDEFS_DYNAMIC( _type ) \
    WRAPDEFS_CLASS() \
    virtual gse::Value* const Wrap( const bool dynamic = false ) override; \
    static _type* Unwrap( gse::Value* const value ); \
    static void WrapSet( gse::Wrappable* wrapobj, const std::string& key, gse::Value* const value, GSE_CALLABLE ); \
    void OnWrapSet( const std::string& property_name );
#define WRAPDEFS_NOPTR( _type ) \
    WRAPDEFS_CLASS() \
    virtual gse::Value* const Wrap( const bool dynamic = false ) override; \
    static _type Unwrap( gse::Value* const value );
#define WRAPIMPL_CLASS( _type ) \
    const gse::value::Object::object_class_t _type::WRAP_CLASS = #_type;
#define WRAPIMPL_BEGIN( _type ) \
    WRAPIMPL_CLASS( _type ) \
    gse::Value* const _type::Wrap( const bool dynamic ) {
#define WRAPIMPL_DYNAMIC_BEGIN( _type ) \
    WRAPIMPL_CLASS( _type ) \
    gse::Value* const _type::Wrap( const bool dynamic ) {
#define WRAPIMPL_DYNAMIC_GETTERS( _type ) \
    WRAPIMPL_DYNAMIC_BEGIN( _type ) \
    const gse::value::object_properties_t properties = {
#define WRAPIMPL_PROPS gse::value::object_properties_t properties = { \
    { \
        "on", \
        NATIVE_CALL( this ) { \
            N_EXPECT_ARGS( 2 ); \
            N_GETVALUE( event, 0, String ); \
            N_GET( cb, 1 ); \
            N_CHECKARG( cb, 1, Callable ); \
            return VALUE( gse::value::Int, On( GSE_CALL, event, cb ) ); \
        } ) \
    }, \
    { \
        "off", \
        NATIVE_CALL( this ) { \
            N_EXPECT_ARGS_MIN_MAX( 1, 2 ); \
            N_GETVALUE( event, 0, String ); \
            if ( arguments.size() == 2 ) { \
                N_GETVALUE( handler_id, 1, Int ); \
                Off( GSE_CALL, event, handler_id ); \
            } \
            else { \
                Off( GSE_CALL, event, 0 ); \
            } \
            return VALUE( gse::value::Undefined ); \
        } ) \
    }, \
    { \
        "trigger", \
        NATIVE_CALL( this ) { \
            N_EXPECT_ARGS_MIN_MAX( 1, 2 ); \
            N_GETVALUE( event, 0, String ); \
            if ( arguments.size() == 2 ) { \
                N_GETVALUE( args, 1, Object ); \
                return Trigger( GSE_CALL, event, args ); \
            } \
            else { \
                return Trigger( GSE_CALL, event, {} ); \
            } \
        } ) \
    },

#define WRAPIMPL_PROPS_EXTEND( _parent ) \
    const auto wrapped_parent = _parent::Wrap(); \
    const auto& wrapped_parent_props = ( (gse::value::Object*)wrapped_parent )->value; \
    properties.insert( wrapped_parent_props.begin(), wrapped_parent_props.end() );
#define WRAPIMPL_END_PTR() \
    return VALUE( gse::value::Object, nullptr, properties, WRAP_CLASS, this ); \
}
#define WRAPIMPL_DYNAMIC_SETTERS( _type ) \
    }; \
    return VALUE( gse::value::Object, nullptr, properties, WRAP_CLASS, this, dynamic ? &_type::WrapSet : nullptr ); \
} \
void _type::WrapSet( gse::Wrappable* wrapobj, const std::string& key, gse::Value* const value, GSE_CALLABLE ) { \
    auto* obj = (_type*)wrapobj; \
    if ( !obj ) {}
#define WRAPIMPL_END_NOPTR( _type ) \
    return VALUE( gse::value::Object, nullptr, properties, WRAP_CLASS ); \
}
#define WRAPIMPL_DYNAMIC_ON_SET( _type ) \
    else { \
        throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property does not exist", GSE_CALL ); \
    } \
} \
void _type::OnWrapSet( const std::string& property_name ) {
#define WRAPIMPL_DYNAMIC_END() \
}
#define WRAPIMPL_GET( _key, _type, _property ) \
    { \
        _key, \
        VALUE( gse::value::_type, _property ) \
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
        if ( value->type != gse::value::_type::GetType() ) { \
            throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Invalid assignment value type, expected: " + gse::Value::GetTypeStringStatic( gse::value::_type::GetType() ) + ", got: " + value->GetTypeString(), GSE_CALL ); \
        } \
        obj->_property = ((gse::value::_type*)value)->value; \
        obj->OnWrapSet( _key ); \
    }

#define UNWRAPIMPL_PTR( _type ) \
_type* _type::Unwrap( gse::Value* const value ) { \
    const auto* valueobj = value->Deref(); \
    ASSERT_NOLOG( valueobj->type == gse::Value::T_OBJECT, "can't unwrap non-object: " + valueobj->Dump() ); \
    const auto* obj = (gse::value::Object*)valueobj; \
    ASSERT_NOLOG( obj->object_class == WRAP_CLASS, "can't unwrap object of different class ( " + obj->object_class + " != " + WRAP_CLASS + " )" ); \
    ASSERT_NOLOG( obj->wrapobj, "can't unwrap object without internal link" ); \
    return (_type*)obj->wrapobj; \
}

#define UNWRAPIMPL_NOPTR_BEGIN( _type ) \
_type _type::Unwrap( gse::Value* const value ) { \
    const auto* valueobj = value->Deref(); \
    ASSERT_NOLOG( valueobj->type == gse::Value::T_OBJECT, "can only unwrap objects" ); \
    const auto* obj = (gse::value::Object*)valueobj; \
    ASSERT_NOLOG( obj->object_class == WRAP_CLASS, "can only unwrap objects of same class" ); \
    const auto& properties = obj->value;

#define UNWRAPIMPL_NOPTR_END() \
}

class Value {
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
		T_VALUEREF,
		T_RANGE,
		T_LOOPCONTROL,
	};

	static const std::string& GetTypeStringStatic( const type_t type );
	const std::string& GetTypeString() const;
	const std::string ToString() const;
	const std::string Dump() const;

	Value* const Deref();
	Value* const Clone();

#define OP( _op ) const bool operator _op( const Value& other ) const;
	OP( == )
	OP( != )
	OP( < )
	OP( > )
	OP( <= )
	OP( >= )
#undef OP

	const type_t type;

	Value* const New( const Value* value );

	static void Serialize( types::Buffer* buf, Value* const type );
	static Value* Unserialize( types::Buffer* buf );

protected:
	Value( const type_t type )
		: type( type ) {}

};

}
