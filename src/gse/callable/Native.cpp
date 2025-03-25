#include "Native.h"

#include "gse/Exception.h"
#include "gc/Space.h"

namespace gse {
namespace callable {

Native::Native( gc::Space* const gc_space, context::Context* const ctx, const executor_t& executor )
	: Callable( gc_space, ctx )
	, m_executor( executor ) {
	//
}

Native::~Native() {

}

Value* Native::Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) {
	CHECKACCUM( m_gc_space );
	return m_executor( GSE_CALL, arguments );
}

}
}
