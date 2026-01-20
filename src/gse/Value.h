#pragma once

#include <memory>
#include <functional>

#include "common/Assert.h"

#include "gc/Object.h"

#include "gse/Types.h"

namespace game {
namespace backend {
class Game;
}
}

namespace gc {
class Space;
}

namespace types {
class Buffer;
}

namespace gse {

namespace context {
class Context;
}

// TODO: move all this shit somewhere

#define GSE_ERROR( _type, _text ) throw gse::Exception( _type, _text, GSE_CALL_NOGC );

#define GSE_CALLABLE_NOGC gse::context::Context* ctx, const gse::si_t& si, gse::ExecutionPointer& ep
#define GSE_CALLABLE gc::Space* const gc_space, GSE_CALLABLE_NOGC
#define GSE_CALL_NOGC ctx, si, ep
#define GSE_CALL gc_space, GSE_CALL_NOGC

typedef std::function< void( GSE_CALLABLE, value::object_properties_t& args ) > f_args_t;

#define VALUE( _type, ... ) ( new _type( gc_space __VA_ARGS__ ) )
#define VALUEEXT( _type, ... ) ( new _type( __VA_ARGS__ ) )
#ifdef DEBUG
#define VALUE_DATA( _type, _var ) ( _var->type == _type::GetType() ? ((_type*)_var) : THROW( "invalid GSE value type (expected " + Value::GetTypeStringStatic( _type::GetType() ) + ", got " + _var->GetTypeString() + ")" ) )
#else
#define VALUE_DATA( _type, _var ) ((_type*)_var)
#endif
#define VALUE_GET( _type, _var ) VALUE_DATA( _type, _var )->value
#define VALUE_SET( _type, _var, _value ) { VALUE_DATA( _type, _var )->value = _value; }
#define VALUE_CLONE( _type, _var ) VALUE( _type,, VALUE_GET( _type, _var ) )

#define WRAPDEF_SERIALIZABLE \
    static void SerializeRef( types::Buffer* const buf, const Wrappable* const wrapobj ); \
    static gse::Value* const DeserializeRef( GSE_CALLABLE, const Game* const game, types::Buffer* const buf );

#define WRAPDEFS_CLASS() \
    static const gse::value::Object::object_class_t WRAP_CLASS;
#define WRAPDEFS_PTR( _type ) \
    WRAPDEFS_CLASS() \
    virtual gse::Value* const Wrap( GSE_CALLABLE, const bool dynamic = false ) override; \
    static _type* Unwrap( gse::Value* const value );
#define WRAPDEFS_DYNAMIC( _type ) \
    WRAPDEFS_CLASS() \
    virtual gse::Value* const Wrap( GSE_CALLABLE, const bool dynamic = false ) override; \
    static _type* Unwrap( gse::Value* const value ); \
    static void WrapSet( gse::Wrappable* wrapobj, const std::string& key, gse::Value* const value, GSE_CALLABLE ); \
    void OnWrapSet( GSE_CALLABLE, const std::string& property_name );
#define WRAPDEFS_NOPTR( _type ) \
    WRAPDEFS_CLASS() \
    virtual gse::Value* const Wrap( GSE_CALLABLE, const bool dynamic = false ) override; \
    static _type Unwrap( gse::Value* const value );
#define WRAPIMPL_CLASS( _type ) \
    const gse::value::Object::object_class_t _type::WRAP_CLASS = #_type;
#define WRAPIMPL_SERIALIZE( _type ) void _type::SerializeRef( types::Buffer* const buf, const Wrappable* const wrapobj ) { \
    const auto* obj = (_type*)wrapobj;
#define WRAPIMPL_DESERIALIZE( _type ) gse::Value* const _type::DeserializeRef( GSE_CALLABLE, const Game* const game, types::Buffer* const buf ) {
#define WRAPIMPL_BEGIN( _type ) \
    WRAPIMPL_CLASS( _type ) \
    gse::Value* const _type::Wrap( GSE_CALLABLE, const bool dynamic ) {
#define WRAPIMPL_DYNAMIC_BEGIN( _type ) \
    WRAPIMPL_CLASS( _type ) \
    gse::Value* const _type::Wrap( GSE_CALLABLE, const bool dynamic ) {
#define WRAPIMPL_DYNAMIC_GETTERS( _type ) \
    WRAPIMPL_DYNAMIC_BEGIN( _type ) \
    const gse::value::object_properties_t properties = {
#define WRAPIMPL_PROPS gse::value::object_properties_t properties = {
#define WRAPIMPL_TRIGGERS \
    { \
        "on", \
        NATIVE_CALL( this ) { \
            N_EXPECT_ARGS( 2 ); \
            N_GETVALUE( event, 0, String ); \
            N_GET( cb, 1, Callable ); \
            return VALUE( gse::value::Int,, On( GSE_CALL, event, cb ) ); \
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
                N_GETVALUE( a, 1, Object ); \
                return Trigger( GSE_CALL, event, ARGS( a ) ); \
            } \
            else { \
                return Trigger( GSE_CALL, event ); \
            } \
        } ) \
    },

#define WRAPIMPL_PROPS_EXTEND( _parent ) \
    const auto wrapped_parent = _parent::Wrap( GSE_CALL ); \
    const auto& wrapped_parent_props = ( (gse::value::Object*)wrapped_parent )->value; \
    properties.insert( wrapped_parent_props.begin(), wrapped_parent_props.end() );
#define WRAPIMPL_END_PTR() \
    return VALUEEXT( gse::value::Object, GSE_CALL, properties, WRAP_CLASS, this ); \
}
#define WRAPIMPL_DYNAMIC_SETTERS( _type ) \
    }; \
    return VALUEEXT( gse::value::Object, GSE_CALL, properties, WRAP_CLASS, this, dynamic ? &_type::WrapSet : nullptr ); \
} \
void _type::WrapSet( gse::Wrappable* wrapobj, const std::string& key, gse::Value* const value, GSE_CALLABLE ) { \
    auto* obj = (_type*)wrapobj; \
    if ( !obj ) { return; }

#define WRAPIMPL_END_NOPTR( _type ) \
    return VALUEEXT( gse::value::Object, GSE_CALL, properties, WRAP_CLASS ); \
}
#define WRAPIMPL_DYNAMIC_ON_SET( _type ) \
    GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Property does not exist" ); \
} \
void _type::OnWrapSet( GSE_CALLABLE, const std::string& property_name ) {
#define WRAPIMPL_DYNAMIC_END() \
}
#define WRAPIMPL_GET_CUSTOM( _key, _type, ... ) \
    { \
        _key, \
        VALUE( gse::value::_type,, __VA_ARGS__ ) \
    },
#define WRAPIMPL_GET( _property, _type ) WRAPIMPL_GET_CUSTOM( #_property, _type, _property )
#define WRAPIMPL_GET_MAPPED_CUSTOM( _property, _map ) \
    { \
        #_property, \
        _map.Get( GSE_CALL, _property ) \
    },
#define WRAPIMPL_GET_MAPPED( _property ) WRAPIMPL_GET_MAPPED_CUSTOM( _property, __wrap_##_property )
#define WRAPIMPL_GET_WRAPPED( _property ) \
    { \
        #_property, \
        _property.Wrap( GSE_CALL, dynamic ) \
    },

#define WRAPIMPL_LINK( _key, _property ) \
    { \
        _key, \
        NATIVE_CALL( this ) { \
            return _property->Wrap( GSE_CALL ); \
        }) \
    },

#define WRAPIMPL_SET_CUSTOM( _key, _type, _property ) \
    if ( key == _key ) { \
        if ( value->type != gse::value::_type::GetType() ) { \
            GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Invalid assignment value type, expected: " + gse::Value::GetTypeStringStatic( gse::value::_type::GetType() ) + ", got: " + value->GetTypeString() ); \
        } \
        obj->_property = ((gse::value::_type*)value)->value; \
        obj->OnWrapSet( GSE_CALL, _key ); \
        return; \
    }
#define WRAPIMPL_SET( _property, _type ) WRAPIMPL_SET_CUSTOM( #_property, _type, _property )

#define WRAPIMPL_SET_MAPPED_CUSTOM( _property, _map ) \
    if ( key == #_property ) { \
        if ( value->type != gse::VT_STRING ) { \
            GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Invalid assignment value type, expected: String, got: " + value->GetTypeString() ); \
        } \
        obj->_property = _map.GetValue( GSE_CALL, ((gse::value::String*)value)->value ); \
        obj->OnWrapSet( GSE_CALL, #_property ); \
        return; \
    }
#define WRAPIMPL_SET_MAPPED( _property ) WRAPIMPL_SET_MAPPED_CUSTOM( _property, __wrap_##_property )

#define WRAPMAP( _name, _type, ... ) \
static const gse::Wrappable::wrapmap_t< _type > __wrap_##_name = { \
    { \
        __VA_ARGS__ \
    } \
};

#define UNWRAPIMPL_PTR( _type ) \
_type* _type::Unwrap( gse::Value* const value ) { \
    const auto* valueobj = value->Deref(); \
    ASSERT( valueobj->type == gse::VT_OBJECT, "can't unwrap non-object: " + valueobj->Dump() ); \
    const auto* obj = (gse::value::Object*)valueobj; \
    ASSERT( !obj->object_class.empty() && ( obj->object_class == WRAP_CLASS || ( WRAP_CLASS == "Object" && obj->object_class[ 0 ] == '$' ) ), "can't unwrap object of different class ( " + obj->object_class + " != " + WRAP_CLASS + " )" ); \
    ASSERT( obj->wrapobj, "can't unwrap object without internal link" ); \
    return (_type*)obj->wrapobj; \
}

#define UNWRAPIMPL_NOPTR_BEGIN( _type ) \
_type _type::Unwrap( gse::Value* const value ) { \
    const auto* valueobj = value->Deref(); \
    ASSERT( valueobj->type == gse::VT_OBJECT, "can only unwrap objects" ); \
    const auto* obj = (gse::value::Object*)valueobj; \
    ASSERT( obj->object_class == WRAP_CLASS, "can only unwrap objects of same class" ); \
    const auto& properties = obj->value;

#define UNWRAPIMPL_NOPTR_END() \
}

class Value : public gc::Object {
public:

	virtual ~Value();

	static const std::string& GetTypeStringStatic( const value_type_t type );
	const std::string& GetTypeString() const;
	virtual const std::string ToString() const;
	const std::string ToStringImpl( std::unordered_set< const Value* >& stack ) const;
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

	value_type_t type = VT_UNDEFINED;

	Value* const New( const Value* value );

	static void Serialize( types::Buffer* buf, const Value* const type );
	static Value* Deserialize( GSE_CALLABLE, types::Buffer* buf, game::backend::Game* const game = nullptr );

protected:
	Value( gc::Space* const gc_space, const value_type_t type );

	gc::Space* const m_gc_space = nullptr;

};

}
