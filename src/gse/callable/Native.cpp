#include "Native.h"

#include "gse/Exception.h"

namespace gse {
namespace callable {

Native::Native( const executor_t& executor )
	: m_executor( executor ) {
	//
}

Value Native::Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) {
	return m_executor( ctx, call_si, arguments );
}

}
}
