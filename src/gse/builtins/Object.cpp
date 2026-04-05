#include "Object.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/value/Object.h"
#include "gse/value/Array.h"
#include "gse/value/String.h"

namespace gse {
namespace builtins {

void Object::AddToContext( gc::Space* const gc_space, context::Context* ctx, gse::ExecutionPointer& ep ) {

	ctx->CreateBuiltin( "keys", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( properties, 0, Object );
		gse::value::array_elements_t elements = {};
		for ( const auto& it : properties ) {
			elements.push_back( VALUE( value::String,, it.first ) );
		}
		return VALUE( value::Array,, elements );
	} ), ep );

}

}
}
