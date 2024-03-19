#include "Math.h"

#include <cmath>

#include "gse/GSE.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"

namespace gse {
namespace builtins {

void Math::AddToContext( gse::Context* ctx ) {

	ctx->CreateBuiltin( "round", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( v, 0, Float );
		return VALUE( type::Int, std::round( v ) );
	} ) );

}

}
}
