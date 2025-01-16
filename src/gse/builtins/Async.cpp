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

		const auto timer_id = ctx->GetGSE()->GetAsync()->StartTimer( ms, arguments.at(1 ), ctx, call_si );

		const gse::type::object_properties_t properties = {
			{
				"stop",
				// recursive NATIVE_CALL doesn't work
				gse::Value( std::make_shared< gse::callable::Native >([ timer_id ]( gse::context::Context* ctx, const gse::si_t& call_si, const gse::type::function_arguments_t& arguments ) -> gse::Value {
					N_EXPECT_ARGS( 0 );
					if ( !ctx->GetGSE()->GetAsync()->StopTimer( timer_id ) ) {
						GSE_ERROR( EC.OPERATION_FAILED, "Timer is already stopped" );
					}
					return VALUE( type::Undefined );
				} ) ),
			}
		};

		return VALUE( type::Object, nullptr, properties, "async" );
	} ) );

}

}
}
