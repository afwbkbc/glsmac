#include "String.h"

#include <algorithm>

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/value/String.h"
#include "gse/value/Undefined.h"

namespace gse {
namespace builtins {

void String::AddToContext( context::Context* ctx, gse::ExecutionPointer& ep ) {

	ctx->CreateBuiltin( "uppercase", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE_NONCONST( text, 0, String );
		std::transform( text.begin(), text.end(), text.begin(), ::toupper );
		return VALUE( value::String, text );
	} ), ep );

	ctx->CreateBuiltin( "lowercase", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE_NONCONST( text, 0, String );
		std::transform( text.begin(), text.end(), text.begin(), ::tolower );
		return VALUE( value::String, text );
	} ), ep );

}

}
}
