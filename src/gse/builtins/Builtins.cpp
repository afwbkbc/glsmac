#include "Builtins.h"

namespace gse {
namespace builtins {

void Builtins::AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) {
	m_common.AddToContext( gc_space, ctx, ep );
	m_include.AddToContext( gc_space, ctx, ep );
	m_console.AddToContext( gc_space, ctx, ep );
	m_conversions.AddToContext( gc_space, ctx, ep );
	m_math.AddToContext( gc_space, ctx, ep );
	m_string.AddToContext( gc_space, ctx, ep );
	m_async.AddToContext( gc_space, ctx, ep );
}

}
}
