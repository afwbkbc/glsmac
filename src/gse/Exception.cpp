#include "Exception.h"

#include "gse/context/Context.h"
#include "util/String.h"

namespace gse {

const exception_ec_t EC = {
	"GSEParseError",
	"GSETypeError",
	"GSEReferenceError",
	"GSEInvalidAssignment",
	"GSEOperationNotSupported",
	"GSEOperationFailed",
	"GSEInvalidDereference",
	"GSEInvalidCall",
	"GSELoaderError",
	"GSEMathError",
	"GSEConversionError",
	"GSEGameError",
	"GSEInvalidEvent",
};

const Exception::backtrace_t Exception::GetBacktraceAndCleanup( const context::Context* const current_ctx ) {
	ASSERT_NOLOG( !contexts_freed, "contexts already freed" );
	context::Context* ctx = context;
	context::Context* pt;
#define FORMAT_SI( _si ) "\tat " + (_si).file + ":" + std::to_string( (_si).from.line ) + ": " + util::String::TrimCopy( ctx ? ctx->GetSourceLine( (_si).from.line ) : "" )
	backtrace_t backtrace = { FORMAT_SI( si ) };
	while ( ctx && ctx != current_ctx ) {
		if ( ctx->IsTraceable() ) {
			backtrace.push_back( FORMAT_SI( ctx->GetSI() ) );
		}
		pt = ctx->GetCallerContext();
		ctx->DecRefs();
		ctx = pt;
	}
#undef FORMAT_SI
	contexts_freed = true;
	return backtrace;
}

const std::string Exception::ToStringAndCleanup() {
	std::string result = "Unhandled exception (" + class_name + "): " + reason + "\n";
	const auto backtrace = GetBacktraceAndCleanup( context );
	bool is_first = true;
	for ( const auto& it : backtrace ) {
		if ( is_first ) {
			is_first = false;
		}
		else {
			result += "\n";
		}
		result += it;
	}
	return result;
}

}
