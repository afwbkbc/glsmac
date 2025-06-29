#include "String.h"

#include <algorithm>

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/value/String.h"
#include "gse/value/Array.h"
#include "util/String.h"

namespace gse {
namespace builtins {

void String::AddToContext( gc::Space* const gc_space, context::Context* ctx, gse::ExecutionPointer& ep ) {

	ctx->CreateBuiltin( "uppercase", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE_NONCONST( text, 0, String );
		std::transform( text.begin(), text.end(), text.begin(), ::toupper );
		return VALUE( value::String,, text );
	} ), ep );

	ctx->CreateBuiltin( "lowercase", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE_NONCONST( text, 0, String );
		std::transform( text.begin(), text.end(), text.begin(), ::tolower );
		return VALUE( value::String,, text );
	} ), ep );

	ctx->CreateBuiltin( "trim", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE_NONCONST( text, 0, String );
		util::String::Trim( text );
		return VALUE( value::String,, text );
	} ), ep );

	ctx->CreateBuiltin( "split", NATIVE_CALL() {
		N_EXPECT_ARGS( 2 );
		N_GETVALUE( text, 0, String );
		N_GETVALUE( sep, 1, String );
		if ( sep.size() != 1 ) {
			GSE_ERROR( EC.INVALID_CALL, "For now splitting is only possible by single character (got: " + sep + ")" );
		}
		const auto results = util::String::Split( text, sep[0] );
		value::array_elements_t elements = {};
		elements.reserve( results.size() );
		for ( const auto& result : results ) {
			elements.push_back( VALUE( value::String,, result ) );
		}
		return VALUE( value::Array,, elements );
	} ), ep );

}

}
}
