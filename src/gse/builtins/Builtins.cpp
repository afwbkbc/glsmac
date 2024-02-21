#include "Builtins.h"

namespace gse {
namespace builtins {

void Builtins::AddToContext( gse::Context* ctx ) {
	m_common.AddToContext( ctx );
	m_include.AddToContext( ctx );
	m_console.AddToContext( ctx );
	m_conversions.AddToContext( ctx );
}

}
}
