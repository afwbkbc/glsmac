#include "Builtins.h"

#include "gse/type/Null.h"

namespace gse {
namespace builtin {

void Builtins::AddToContext( gse::Context* ctx ) const {
	m_include.AddToContext( ctx );
	m_console.AddToContext( ctx );
}

}
}
