#pragma once

#include <functional>

#include "gse/type/Callable.h"
#include "gse/type/Object.h"
#include "gse/type/Undefined.h"
#include "gse/type/Bool.h"

namespace gse {
namespace callable {

#define NATIVE_CALL( ... ) VALUE( gse::callable::Native, [ __VA_ARGS__ ]( gse::Context* ctx, const gse::si_t& call_si, const gse::type::Callable::function_arguments_t& arguments ) -> gse::Value

// TODO: refactor these
#define N_ARGS \
    const gse::type::Type* arg; \
    gse::type::Object::properties_t::const_iterator obj_it; \
    auto getprop_val = VALUE( gse::type::Undefined ); \
    auto obj_val = VALUE( gse::type::Undefined );
#define N_EXPECT_ARGS( _count ) \
    if ( arguments.size() != _count ) { \
        throw gse::Exception( gse::EC.INVALID_CALL, "Expected " + std::to_string( _count ) + " arguments, found " + std::to_string( arguments.size() ), ctx, call_si ); \
    } \
    N_ARGS
#define N_EXPECT_ARGS_MIN_MAX( _min, _max ) \
    if ( arguments.size() < _min || arguments.size() > _max ) { \
        throw gse::Exception( gse::EC.INVALID_CALL, "Expected " + std::to_string( _min ) + " to " + std::to_string( _max ) + " arguments, found " + std::to_string( arguments.size() ), ctx, call_si ); \
    } \
    N_ARGS
#define N_GET( _var, _index ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    const auto& _var = arguments.at( _index );
#define N_GETPTR( _var, _index ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    const auto* _var = arguments.at( _index ).Get();
#define N_CHECKTYPE( _var, _index, _type ) \
    if ( _var->type != gse::type::_type::GetType() ) { \
        throw gse::Exception( gse::EC.INVALID_CALL, "Argument " + std::to_string( _index ) + " is expected to be " + #_type + ", found: " + _var->GetTypeString( _var->type ), ctx, call_si ); \
    }
#define N_GETVALUE( _var, _index, _type ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    arg = arguments.at( _index ).Get(); \
    N_CHECKTYPE( arg, _index, _type ); \
    const auto& _var = ((gse::type::_type*)arg)->value;
#define N_GETOBJ( _index, _class ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    arg = arguments.at( _index ).Get(); \
    N_CHECKTYPE( arg, _index, Object ); \
    if ( ((gse::type::Object*)arg)->object_class != _class ) { \
        throw gse::Exception( gse::EC.INVALID_CALL, "Argument " + std::to_string( _index ) + " is expected to be object of class " + gse::type::Object::GetClassString( _class ) + ", found class: " + gse::type::Object::GetClassString( ((gse::type::Object*)arg)->object_class ), ctx, call_si ); \
    } \
    obj_val = arguments.at( _index );
#define N_UNWRAP( _var, _index, _type ) \
    N_GETOBJ( _index, _type::WRAP_CLASS ); \
    const auto* _var = _type::Unwrap( obj_val );
#define N_CHECK_OBJECT_CLASS( _var, _class ) \
    if ( ((gse::type::Object*)_var)->object_class != _class ) { \
        throw gse::Exception( gse::EC.INVALID_CALL, "Value is expected to be object of class " + gse::type::Object::GetClassString( _class ) + ", found class: " + gse::type::Object::GetClassString( ((gse::type::Object*)_var)->object_class ), ctx, call_si ); \
    }
#define N_GETOBJECT( _var, _index, _class ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    arg = arguments.at( _index ).Get(); \
    N_CHECKTYPE( arg, _index, Object ); \
    N_CHECK_OBJECT_CLASS( arg, _class ); \
    const auto& _var = ((gse::type::Object*)arg)->value;
#define N_GETPROP_VAL( _obj, _key, _type ) \
    obj_it = _obj.find( _key ); \
    if ( obj_it == _obj.end() ) { \
        throw gse::Exception( gse::EC.INVALID_CALL, (std::string)"Property '" + _key + "' is expected but not found", ctx, call_si ); \
    } \
    getprop_val = obj_it->second;
#define N_GETPROP_ARG( _obj, _key, _type ) \
    N_GETPROP_VAL( _obj, _key, _type ); \
    arg = getprop_val.Get(); \
    if ( arg->type != gse::type::_type::GetType() ) { \
        throw gse::Exception( gse::EC.INVALID_CALL, (std::string)"Property '" + _key + "' is expected to be " + #_type + ", found: " + arg->GetTypeString( arg->type ), ctx, call_si ); \
    }
#define N_GETPROP_UNWRAP( _var, _obj, _key, _type ) \
    N_GETPROP_VAL( _obj, _key, Object ); \
    N_CHECK_OBJECT_CLASS( getprop_val.Get(), _type::WRAP_CLASS ); \
    const auto _var = _type::Unwrap( getprop_val );
#define N_GETPROP( _var, _obj, _key, _type ) \
    N_GETPROP_ARG( _obj, _key, _type ); \
    const auto& _var = ((gse::type::_type*)arg)->value;
#define N_GETPROP_OBJECT( _var, _obj, _key, _class ) \
    N_GETPROP_ARG( _obj, _key, Object ); \
    N_CHECK_OBJECT_CLASS( arg, gse::type::Object::_class ); \
    const auto& _var = ((gse::type::Object*)arg)->value;
#define N_GETPROP_OPTBOOL( _var, _obj, _key ) \
    bool _var = false; \
    obj_it = _obj.find( _key ); \
    if ( obj_it != _obj.end() ) { \
        getprop_val = obj_it->second; \
        arg = getprop_val.Get(); \
        if ( arg->type != gse::type::Type::T_BOOL ) { \
            throw gse::Exception( gse::EC.INVALID_CALL, (std::string)"Property '" + _key + "' is expected to be bool, found: " + arg->GetTypeString( arg->type ), ctx, call_si ); \
        } \
        _var = ((gse::type::Bool*)arg)->value; \
    }

class Native : public type::Callable {
public:
	typedef std::function< Value( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) > executor_t;
	Native() = delete;
	Native( const executor_t& executor );
	virtual Value Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override;

private:
	const executor_t m_executor;
};

}
}