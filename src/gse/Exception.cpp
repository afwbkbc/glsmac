#include "Exception.h"

#include "util/String.h"

namespace gse {

const Exception::backtrace_t Exception::GetBacktraceAndCleanup( const Context* const current_ctx ) {
	ASSERT_NOLOG( !contexts_freed, "contexts already freed" );
	const Context* ctx = context, * pt;
#define FORMAT_SI( _si ) "\tat " + (_si).file + ":" + std::to_string( (_si).from.line ) + ": " + util::String::TrimCopy( ctx->GetSourceLine( (_si).from.line ) )
	backtrace_t backtrace = { FORMAT_SI( si ) };
	while ( ctx != current_ctx ) {
		backtrace.push_back( FORMAT_SI( ctx->GetSI() ) );
		pt = ctx->GetParentContext();
		delete ctx;
		ctx = pt;
	}
#undef FORMAT_SI
	contexts_freed = true;
	return backtrace;
}

}
