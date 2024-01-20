#pragma once

#include <functional>

#include "gse/type/Callable.h"

namespace gse {
namespace callable {

#define NATIVE_CALL( ... ) VALUE( gse::callable::Native, [ __VA_ARGS__ ]( gse::Context* ctx, const gse::si_t& call_si, const type::Callable::function_arguments_t& arguments ) -> Value
#define N_EXPECT_ARGS( _count ) \
    if ( arguments.size() != _count ) { \
        throw gse::Exception( EC.INVALID_CALL, "Expected " + std::to_string( _count ) + " arguments, found " + std::to_string( arguments.size() ), ctx, call_si ); \
    } \
    const type::Type* arg;
#define N_GET( _var, _index ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    const auto& _var = arguments.at( _index );
#define N_GETVALUE( _var, _index, _type ) \
    ASSERT_NOLOG( _index < arguments.size(), "argument index overflow" ); \
    arg = arguments.at( _index ).Get(); \
    if ( arg->type != gse::type::_type::GetType() ) { \
        throw gse::Exception( EC.INVALID_CALL, "Argument " + std::to_string( _index ) + " is expected to be " + #_type + ", found: " + arg->GetTypeString( arg->type ), ctx, call_si );\
    } \
    const auto& _var = ((gse::type::_type*)arg)->value;

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