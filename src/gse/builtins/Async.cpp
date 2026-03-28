#include "Async.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/value/Exception.h"
#include "gse/value/Int.h"
#include "gse/value/Undefined.h"
#include "gse/GSE.h"
#include "gse/Async.h"

namespace gse {
namespace builtins {

void Async::AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) {

	ctx->CreateBuiltin( "async", NATIVE_CALL() {
		N_EXPECT_ARGS( 2 );
		N_GETVALUE( ms, 0, Int );
		N_GET_CALLABLE( f, 1 );
		return ctx->GetGSE()->GetAsync()->CreateTimer( ms, f, GSE_CALL_NOGC );
	} ), ep );

}

}
}
