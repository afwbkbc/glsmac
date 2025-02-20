#include "Include.h"

#include "gse/GSE.h"
#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/value/Undefined.h"
#include "gse/value/String.h"

#include "util/FS.h"

namespace gse {
namespace builtins {

void Include::AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) {

	ctx->CreateBuiltin( "include", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( path, 0, String );
		const auto full_path = ctx->GetScriptInfo().directory + GSE::PATH_SEPARATOR + path;
		return ctx->GetGSE()->RunScript( GSE_CALL, full_path );
	} ), gc_space, ep );

}

}
}
