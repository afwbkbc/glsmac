#pragma once

#include <functional>

#include "gse/value/Callable.h"
#include "gse/value/Types.h"

#include "gse/Value.h"

namespace gse {
namespace callable {

#define NATIVE_CALL( ... ) VALUE( gse::callable::Native,, ctx, [ __VA_ARGS__ ]( GSE_CALLABLE, const gse::value::function_arguments_t& arguments ) -> gse::Value*

// TODO: refactor these
#define N_ARGS \
    const gse::Value* arg; \
    gse::value::object_properties_t::const_iterator obj_it; \
    gse::Value* getprop_val = VALUE( gse::value::Undefined ); \
    gse::Value* obj_val = VALUE( gse::value::Undefined ); \
    gse::Value* callable_val = VALUE( gse::value::Undefined );
#define N_EXPECT_ARGS( _count ) \
    if ( arguments.size() != _count ) { \
        GSE_ERROR( gse::EC.INVALID_CALL, "Expected " + std::to_string( _count ) + " arguments, found " + std::to_string( arguments.size() ) ); \
    } \
    N_ARGS
#define N_EXPECT_ARGS_MIN( _min ) \
    if ( arguments.size() < _min ) { \
        GSE_ERROR( gse::EC.INVALID_CALL, "Expected at least " + std::to_string( _min ) + " arguments, found " + std::to_string( arguments.size() ) ); \
    } \
    N_ARGS
#define N_EXPECT_ARGS_MIN_MAX( _min, _max ) \
    if ( arguments.size() < _min || arguments.size() > _max ) { \
        GSE_ERROR( gse::EC.INVALID_CALL, "Expected " + std::to_string( _min ) + " to " + std::to_string( _max ) + " arguments, found " + std::to_string( arguments.size() ) ); \
    } \
    N_ARGS
#define N_GET( _var, _index ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    const auto& _var = arguments.at( _index );
#define N_GETPTR( _var, _index ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    const auto* _var = arguments.at( _index );
#define N_CHECKTYPE( _var, _type ) \
    if ( _var->type != gse::value::_type::GetType() ) { \
        GSE_ERROR( gse::EC.INVALID_CALL, (std::string) "Value is expected to be " + #_type + ", found: " + _var->GetTypeString() ); \
    }
#define N_CHECKARG( _var, _index, _type ) \
    if ( _var->type != gse::value::_type::GetType() ) { \
        GSE_ERROR( gse::EC.INVALID_CALL, "Argument " + std::to_string( _index ) + " is expected to be " + #_type + ", found: " + _var->GetTypeString() ); \
    }
#define N_GET_CALLABLE( _var, _index ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    callable_val = arguments.at( _index ); \
    arg = callable_val; \
    N_CHECKARG( arg, _index, Callable ); \
    const auto& _var = (gse::value::Callable*)arg;
#define N_GETVALUE_NONCONST( _var, _index, _type ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    arg = arguments.at( _index )->Deref(); \
    N_CHECKARG( arg, _index, _type ); \
    auto _var = ((gse::value::_type*)arg)->value;
#define N_GETELEMENT( _var, _arr, _index, _type ) \
    ASSERT_NOLOG( _index < _arr.size(), #_arr " index overflow" ); \
    arg = _arr.at( _index )->Deref(); \
    N_CHECKARG( arg, _index, _type ); \
    const auto& _var = ((gse::value::_type*)arg)->value;
#define N_GETVALUE( _var, _index, _type ) \
    N_GETELEMENT( _var, arguments, _index, _type );
#define N_GETOBJ( _value, _class ) \
    arg = _value->Deref(); \
    N_CHECKTYPE( arg, Object ); \
    if ( ((gse::value::Object*)arg)->object_class != _class ) { \
        GSE_ERROR( gse::EC.INVALID_CALL, "Value is expected to be object of class " + _class + ", found class: " + ((gse::value::Object*)arg)->object_class ); \
    } \
    obj_val = _value;
#define N_UNWRAP( _var, _value, _type ) \
    N_GETOBJ( _value, _type::WRAP_CLASS ); \
    auto* _var = _type::Unwrap( obj_val );
#define N_GETVALUE_OBJ( _index, _class ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    arg = arguments.at( _index )->Deref(); \
    N_CHECKARG( arg, _index, Object ); \
    if ( ((gse::value::Object*)arg)->object_class != _class ) { \
        GSE_ERROR( gse::EC.INVALID_CALL, "Argument " + std::to_string( _index ) + " is expected to be object of class " + _class + ", found class: " + ((gse::value::Object*)arg)->object_class ); \
    } \
    obj_val = arguments.at( _index );
#define N_GETVALUE_UNWRAP( _var, _index, _type ) \
    N_GETVALUE_OBJ( _index, _type::WRAP_CLASS ); \
    auto* _var = _type::Unwrap( obj_val );
#define N_CHECK_OBJECT_CLASS( _var, _class ) \
    if ( ((gse::value::Object*)_var)->object_class != _class ) { \
        GSE_ERROR( gse::EC.INVALID_CALL, "Value is expected to be object of class " + _class + ", found class: " + ((gse::value::Object*)_var)->object_class ); \
    }
#define N_GETPROP_VAL( _obj, _key, _type ) \
    obj_it = _obj.find( _key ); \
    if ( obj_it == _obj.end() ) { \
        GSE_ERROR( gse::EC.INVALID_CALL, (std::string)"Property '" + _key + "' is expected but not found" ); \
    } \
    getprop_val = obj_it->second;
#define N_GETPROP_ARG( _obj, _key, _type ) \
    N_GETPROP_VAL( _obj, _key, _type ); \
    arg = getprop_val->Deref(); \
    if ( arg->type != gse::value::_type::GetType() ) { \
        GSE_ERROR( gse::EC.INVALID_CALL, (std::string)"Property '" + _key + "' is expected to be " + #_type + ", found: " + arg->GetTypeString() ); \
    }
#define N_GETPROP_UNWRAP( _var, _obj, _key, _type ) \
    N_GETPROP_VAL( _obj, _key, Object ); \
    N_CHECK_OBJECT_CLASS( getprop_val->Deref(), _type::WRAP_CLASS ); \
    const auto _var = _type::Unwrap( getprop_val );
#define N_GETPROP( _var, _obj, _key, _type ) \
    N_GETPROP_ARG( _obj, _key, _type ); \
    const auto& _var = ((gse::value::_type*)arg)->value;
#define N_GETPROP_OBJECT( _var, _obj, _key, _class ) \
    N_GETPROP_ARG( _obj, _key, Object ); \
    N_CHECK_OBJECT_CLASS( arg, gse::value::Object::_class ); \
    const auto& _var = ((gse::value::Object*)arg)->value;
#define N_GETPROP_OPT( _vartype, _var, _obj, _key, _type, _default ) \
    _vartype _var = _default; \
    obj_it = _obj.find( _key ); \
    if ( obj_it != _obj.end() ) { \
        getprop_val = obj_it->second; \
        arg = getprop_val->Deref(); \
        if ( arg->type != gse::value::_type::GetType() ) { \
            GSE_ERROR( gse::EC.INVALID_CALL, (std::string)"Property '" + _key + "' is expected to be " + #_type + ", found: " + arg->GetTypeString() ); \
        } \
        _var = ((gse::value::_type*)arg)->value; \
    }
#define N_GETPROP_OPT_BOOL( _var, _obj, _key ) N_GETPROP_OPT( bool, _var, _obj, _key, Bool, false )
#define N_GETPROP_OPT_INT( _var, _obj, _key ) N_GETPROP_OPT( int64_t, _var, _obj, _key, Int, 0 )

class Native : public value::Callable {
public:
	typedef std::function< Value*( GSE_CALLABLE, const value::function_arguments_t& arguments ) > executor_t;
	Native() = delete;
	Native( gc::Space* const gc_space, context::Context* const ctx, const executor_t& executor );
	virtual Value* Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) override;

private:
	const executor_t m_executor = nullptr;
};

}
}
