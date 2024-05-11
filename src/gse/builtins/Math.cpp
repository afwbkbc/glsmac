#include "Math.h"

#include <cmath>

#include "gse/GSE.h"
#include "gse/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/Undefined.h"

namespace gse {
namespace builtins {

void Math::AddToContext( gse::Context* ctx ) {

#define AB( _name, _f_int, _f_float ) \
	ctx->CreateBuiltin( _name, NATIVE_CALL() { \
		N_EXPECT_ARGS( 2 ); \
		for ( uint8_t i = 0 ; i < 2 ; i++ ) { \
			const auto& value = arguments.at(i).Get(); \
			if ( value->type != gse::type::Type::T_INT && value->type != gse::type::Type::T_FLOAT ) { \
				throw gse::Exception( gse::EC.INVALID_CALL, "Argument " + std::to_string( i ) + " is expected to be " + gse::type::Type::GetTypeString( gse::type::Type::T_INT ) + " or " + gse::type::Type::GetTypeString( gse::type::Type::T_FLOAT ) + ", found: " + gse::type::Type::GetTypeString( value->type ), ctx, call_si ); \
			} \
		} \
		const auto& a = arguments.at(0).Get(); \
		const auto& b = arguments.at(1).Get(); \
		if ( a->type != b->type ) { \
			throw gse::Exception( gse::EC.INVALID_CALL, "Arguments are of different types: " + gse::type::Type::GetTypeString( a->type ) + ", " + gse::type::Type::GetTypeString( b->type ), ctx, call_si ); \
		} \
		switch ( a->type ) { \
			case gse::type::Type::T_INT: \
				return VALUE( type::Int, _f_int( ((gse::type::Int*)a)->value, ((gse::type::Int*)b)->value ) ); \
			case gse::type::Type::T_FLOAT: \
				return VALUE( type::Float, _f_float( ((gse::type::Float*)a)->value, ((gse::type::Float*)b)->value ) ); \
			default: \
				THROW( "unexpected type: " + gse::type::Type::GetTypeString( a->type ) ); \
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
