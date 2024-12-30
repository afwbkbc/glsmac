#include "Async.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/type/Exception.h"
#include "gse/type/Int.h"
#include "gse/type/Undefined.h"
#include "gse/GSE.h"
#include "gse/Async.h"

namespace gse {
namespace builtins {

void Async::AddToContext( context::Context* ctx ) {

	ctx->CreateBuiltin( "async", NATIVE_CALL() {
		N_EXPECT_ARGS( 2 );
		N_GETVALUE( ms, 0, Int );

		ctx->GetGSE()->GetAsync()->AddTimer( ms, arguments.at(1 ), ctx, call_si );

		return VALUE( type::Undefined );
	} ) );

}

}
}
