#include "Builtins.h"

namespace gse {
namespace builtins {

void Builtins::AddToContext( context::Context* ctx, ExecutionPointer& ep ) {
	m_common.AddToContext( ctx, ep );
	m_include.AddToContext( ctx, ep );
	m_console.AddToContext( ctx, ep );
	m_conversions.AddToContext( ctx, ep );
	m_math.AddToContext( ctx, ep );
	m_string.AddToContext( ctx, ep );
	m_async.AddToContext( ctx, ep );
}

}
}
