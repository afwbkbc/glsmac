#include "Native.h"

#include "gse/Exception.h"

namespace gse {
namespace callable {

Native::Native( gc::Space* const gc_space, const executor_t& executor )
	: Callable( gc_space )
	, m_executor( executor ) {
	//
}

Value* Native::Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) {
	return m_executor( GSE_CALL, arguments );
}

}
}
