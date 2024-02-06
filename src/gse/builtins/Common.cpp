#include "Common.h"

#include "gse/GSE.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/type/Array.h"

namespace gse {
namespace builtins {

void Common::AddToContext( gse::Context* ctx ) {

	ctx->CreateBuiltin( "size", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		size_t size = 0;
		switch ( v->type ) {
			case type::Type::T_ARRAY: {
				size = ((type::Array*)v)->value.size();
				break;
			}
			default:
				throw gse::Exception( EC.OPERATION_NOT_SUPPORTED, "Could not get size of " + v->GetTypeString( v->type ) + ": " + v->ToString(), ctx, call_si );
		}
		return VALUE( type::Int, size );
	} ) );

}

}
}
