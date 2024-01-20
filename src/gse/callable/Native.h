#pragma once

#include <functional>

#include "gse/type/Callable.h"

namespace gse {
namespace callable {

#define NATIVE_CALL( ... ) VALUE( gse::callable::Native, [ __VA_ARGS__ ]( gse::Context* ctx, const gse::si_t& call_si, const type::Callable::function_arguments_t& arguments ) -> Value

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