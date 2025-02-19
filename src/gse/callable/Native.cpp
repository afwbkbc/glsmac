#include "Native.h"

#include "gse/Exception.h"

namespace gse {
namespace callable {

Native::Native( const executor_t& executor )
	: Callable( false )
	, m_executor( executor ) {
	//
}

Value* Native::Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) {
	return m_executor( GSE_CALL, arguments );
}

}
}
