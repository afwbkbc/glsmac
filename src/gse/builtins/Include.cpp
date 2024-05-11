#include "Include.h"

#include "gse/GSE.h"
#include "gse/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Undefined.h"
#include "gse/type/String.h"

#include "util/FS.h"

namespace gse {
namespace builtins {

void Include::AddToContext( gse::Context* ctx ) {

	ctx->CreateBuiltin( "include", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( path, 0, String );
		const auto full_path = ctx->GetScriptInfo().directory + GSE::PATH_SEPARATOR + path;
		return ctx->GetGSE()->GetInclude( ctx, call_si, full_path );
	} ) );

}

}
}
