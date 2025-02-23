#include "Exception.h"

#ifdef DEBUG
#include <unordered_set>
#endif

#include "gse/context/Context.h"
#include "gse/ExecutionPointer.h"
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

Exception::Exception( const std::string& class_name, const std::string& reason, GSE_CALLABLE_NOGC )
	: types::Exception( class_name, reason )
	, class_name( class_name )
	, reason( reason )
	, si( si ) {
	ep.WithSI(
		si, [ this, &ep, &ctx, &si ]() {
			const auto& st = ep.GetStackTrace();
			const auto& ctx_si = ctx
				? ctx->GetSI()
				: si;
			for ( auto s = st.rbegin() ; s != st.rend() ; s++ ) {
				auto* c = s->file == ctx_si.file
					? ctx
					: !s->file.empty()
						? ctx->GetGSE()->GetContextByPath( s->file )
						: nullptr;
				m_stacktrace.push_back(
					"\tat " + s->file + ":" + std::to_string( s->from.line ) + ": " + util::String::TrimCopy(
						c
							? c->GetSourceLine( s->from.line )
							: ""
					)
				);
			}
		}
	);

}

const Exception::stacktrace_t& Exception::GetStackTrace() const {
	return m_stacktrace;
}

const std::string Exception::ToString() const {
	std::string result = "Unhandled exception (" + class_name + "): " + reason + "\n";
	bool is_first = true;
	for ( const auto& it : m_stacktrace ) {
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
