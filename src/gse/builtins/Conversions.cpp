#include "Conversions.h"

#include "gse/GSE.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/String.h"

#include "util/FS.h"

namespace gse {
namespace builtins {

void Conversions::AddToContext( gse::Context* ctx ) {
#define CONVERSION_ERROR( _type ) throw gse::Exception( EC.CONVERSION_ERROR, "Could not convert " + v->GetTypeString(v->type) + " to " + _type + ": " + v->ToString(), ctx, call_si );
	ctx->CreateVariable( "#to_string", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GET( v, 0 );
		return VALUE( type::String, v.ToString() );
	} ), nullptr );
	ctx->CreateVariable( "#to_int", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		int64_t value = 0;
		switch ( v->type ) {
			case type::Type::T_INT: {
				value = ( (type::Int*)v )->value;
				break;
			}
			case type::Type::T_STRING: {
				try {
					value = std::stol( ((type::String*)v)->value );
				}
				catch ( std::logic_error const& ex ) {
					CONVERSION_ERROR( "Int" )
				}
				break;
			}
			default:
				CONVERSION_ERROR( "Int" );
		}
		return VALUE( type::Int, value );
	} ), nullptr );
	ctx->CreateVariable( "#to_float", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		float value = 0.0f;
		switch ( v->type ) {
			case type::Type::T_FLOAT: {
				value = ( (type::Float*)v)->value;
				break;
			}
			case type::Type::T_INT: {
				value = ( (type::Int*)v)->value;
				break;
			}
			case type::Type::T_STRING: {
				try {
					value = std::stof( ((type::String*)v)->value );
				}
				catch ( std::logic_error const& ex ) {
					CONVERSION_ERROR( "Float" )
				}
				break;
			}
			default:
				CONVERSION_ERROR( "Float" );
		}
		return VALUE( type::Float, value );
	} ), nullptr );
#undef CONVERSION_ERROR
}

}
}
