#include "Exception.h"

#ifdef DEBUG
#include <unordered_set>
#endif

#include "gse/context/Context.h"
#include "gse/GSE.h"
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
	"GSEInvalidHandler",
};

Exception::Exception( const std::string& class_name, const std::string& reason, context::Context* context, const si_t& si )
	: types::Exception( class_name, reason )
	, class_name( class_name )
	, reason( reason )
	, context( context )
	, si( si ) {
	auto* ctx = context;

#ifdef DEBUG
	std::unordered_set< const gse::context::Context* > existing_contexts = { ctx };
#endif

#define FORMAT_SI( _si ) "\tat " + (_si).file + ":" + std::to_string( (_si).from.line ) + ": " + util::String::TrimCopy( ctx ? ctx->GetSourceLine( (_si).from.line ) : "" )
	m_backtrace.push_back( FORMAT_SI( si ) );
	while ( ctx ) {
		if ( ctx->IsTraceable() ) {
			const auto& s = ctx->GetSI();
			m_backtrace.push_back( FORMAT_SI( s ) );
		}
		ASSERT_NOLOG( ctx != ctx->GetCallerContext(), "ctx is caller of itself" );
		ctx = ctx->GetCallerContext();
#ifdef DEBUG
		ASSERT_NOLOG( existing_contexts.find( ctx ) == existing_contexts.end(), "duplicate ctx in caller chain" );
		existing_contexts.insert( ctx );
#endif
	}
#undef FORMAT_SI

}

const Exception::backtrace_t& Exception::GetBacktrace( const context::Context* const current_ctx ) {
	return m_backtrace;
}

const std::string Exception::ToString() {
	std::string result = "Unhandled exception (" + class_name + "): " + reason + "\n";
	bool is_first = true;
	for ( const auto& it : m_backtrace ) {
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
