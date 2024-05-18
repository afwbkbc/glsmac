#include "Math.h"

#include <cmath>

#include "gse/GSE.h"
#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/Undefined.h"

namespace gse {
namespace builtins {

void Math::AddToContext( context::Context* ctx ) {

#define AB( _name, _f_int, _f_float ) \
	ctx->CreateBuiltin( _name, NATIVE_CALL() { \
		N_EXPECT_ARGS( 2 ); \
		for ( uint8_t i = 0 ; i < 2 ; i++ ) { \
			const auto& value = arguments.at(i).Get(); \
			if ( value->type != type::Type::T_INT && value->type != type::Type::T_FLOAT ) { \
				throw Exception( EC.INVALID_CALL, "Argument " + std::to_string( i ) + " is expected to be " + type::Type::GetTypeString( type::Type::T_INT ) + " or " + type::Type::GetTypeString( type::Type::T_FLOAT ) + ", found: " + type::Type::GetTypeString( value->type ), ctx, call_si ); \
			} \
		} \
		const auto& a = arguments.at(0).Get(); \
		const auto& b = arguments.at(1).Get(); \
		if ( a->type != b->type ) { \
			throw Exception( EC.INVALID_CALL, "Arguments are of different types: " + type::Type::GetTypeString( a->type ) + ", " + type::Type::GetTypeString( b->type ), ctx, call_si ); \
		} \
		switch ( a->type ) { \
			case type::Type::T_INT: \
				return VALUE( type::Int, _f_int( ((type::Int*)a)->value, ((type::Int*)b)->value ) ); \
			case type::Type::T_FLOAT: \
				return VALUE( type::Float, _f_float( ((type::Float*)a)->value, ((type::Float*)b)->value ) ); \
			default: \
				THROW( "unexpected type: " + type::Type::GetTypeString( a->type ) ); \
		} \
	} ) );
	AB( "min", std::min, std::fmin )
	AB( "max", std::max, std::fmax )
#undef AB

	ctx->CreateBuiltin( "round", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( v, 0, Float );
		return VALUE( type::Int, std::round( v ) );
	} ) );

}

}
}
