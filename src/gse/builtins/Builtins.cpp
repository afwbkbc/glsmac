#include "Builtins.h"

namespace gse {
namespace builtins {

void Builtins::AddToContext( context::Context* ctx ) {
	m_common.AddToContext( ctx );
	m_include.AddToContext( ctx );
	m_console.AddToContext( ctx );
	m_conversions.AddToContext( ctx );
	m_math.AddToContext( ctx );
	m_string.AddToContext( ctx );
}

}
}
