#include "Math.h"

#include <cmath>

#include "gse/GSE.h"
#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/value/Int.h"
#include "gse/value/Float.h"
#include "util/random/Random.h"

namespace gse {
namespace builtins {

Math::Math() {
	m_random = new util::random::Random();
}
Math::~Math() {
	delete m_random;
}

void Math::AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) {

#define AB( _name, _f_int, _f_float ) \
	ctx->CreateBuiltin( _name, NATIVE_CALL() { \
		N_EXPECT_ARGS( 2 ); \
		for ( uint8_t i = 0 ; i < 2 ; i++ ) { \
			const auto& value = arguments.at(i); \
			if ( value->type != VT_INT && value->type != VT_FLOAT ) { \
				GSE_ERROR( EC.INVALID_CALL, "Argument " + std::to_string( i ) + " is expected to be " + Value::GetTypeStringStatic( VT_INT ) + " or " + Value::GetTypeStringStatic( VT_FLOAT ) + ", found: " + value->GetTypeString() ); \
			} \
		} \
		const auto& a = arguments.at(0); \
		const auto& b = arguments.at(1); \
		if ( a->type != b->type ) { \
			GSE_ERROR( EC.INVALID_CALL, "Arguments are of different types: " + a->GetTypeString() + ", " + b->GetTypeString() ); \
		} \
		switch ( a->type ) { \
			case VT_INT: \
				return VALUE( value::Int,, _f_int( ((value::Int*)a)->value, ((value::Int*)b)->value ) ); \
			case VT_FLOAT: \
				return VALUE( value::Float,, _f_float( ((value::Float*)a)->value, ((value::Float*)b)->value ) ); \
			default: \
				THROW( "unexpected type: " + a->GetTypeString() ); \
		} \
	} ), ep );
	AB( "min", std::min, std::fmin )
	AB( "max", std::max, std::fmax )
#undef AB

#define F( _func, _in_type, _out_type ) \
	ctx->CreateBuiltin( #_func, NATIVE_CALL() { \
		N_EXPECT_ARGS( 1 ); \
		N_GETVALUE( v, 0, _in_type ); \
		return VALUE( value::_out_type,, std::_func( v ) ); \
	} ), ep );
	F( floor, Float, Int )
	F( round, Float, Int )
	F( ceil, Float, Int )
	F( sqrt, Float, Float )
#undef F

	ctx->CreateBuiltin( "pow", NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 2 );
		switch ( arguments.at( 0 )->type ) {
			case VT_INT: {
				switch ( arguments.at( 1 )->type ) {
					case VT_INT: {
						N_GETVALUE( a, 0, Int );
						N_GETVALUE( b, 1, Int );
						return VALUE( gse::value::Int,, pow( a, b ) );
					}
					case VT_FLOAT: {
						N_GETVALUE( a, 0, Int );
						N_GETVALUE( b, 1, Float );
						return VALUE( gse::value::Float,, pow( a, b ) );
					}
					default:
						GSE_ERROR( EC.INVALID_CALL, "Second argument must be Int or Float" );
				}
				break;
			}
			case VT_FLOAT: {
				switch ( arguments.at( 1 )->type ) {
					case VT_INT: {
						N_GETVALUE( a, 0, Float );
						N_GETVALUE( b, 1, Int );
						return VALUE( gse::value::Float,, pow( a, b ) );
					}
					case VT_FLOAT: {
						N_GETVALUE( a, 0, Float );
						N_GETVALUE( b, 1, Float );
						return VALUE( gse::value::Float,, pow( a, b ) );
					}
					default:
						GSE_ERROR( EC.INVALID_CALL, "Second argument must be Int or Float" );
				}
				break;
			}
			default:
				GSE_ERROR( EC.INVALID_CALL, "First argument must be Int or Float" );
		}
	} ), ep );

	ctx->CreateBuiltin( "abs", NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 1 );
		switch ( arguments.at( 0 )->type ) {
			case VT_INT: {
				N_GETVALUE( a, 0, Int );
				return VALUE( gse::value::Int,, std::abs( a ) );
			}
			case VT_FLOAT: {
				N_GETVALUE( a, 0, Float );
				return VALUE( gse::value::Float,, std::fabs( a ) );
			}
			default: {
				GSE_ERROR( EC.INVALID_CALL, "Argument must be Int or Float, got: " + arguments.at( 0 )->ToString() );
			}
		}
	} ), ep );

	ctx->CreateBuiltin( "random_int", NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 2 );
		N_GETVALUE( min, 0, Int );
		N_GETVALUE( max, 1, Int );
		if ( min > max ) {
			GSE_ERROR( EC.INVALID_CALL, "Min is larger than max ( " + std::to_string( min ) + " > " + std::to_string( max ) + " )" );
		}
		return VALUE( value::Int,, m_random->GetInt64( min, max ) );
	} ), ep );

	ctx->CreateBuiltin( "random_float", NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 2 );
		N_GETVALUE( min, 0, Float );
		N_GETVALUE( max, 1, Float );
		if ( min > max ) {
			GSE_ERROR( EC.INVALID_CALL, "Min is larger than max ( " + std::to_string( min ) + " > " + std::to_string( max ) + " )" );
		}
		return VALUE( value::Float,, m_random->GetFloat( min, max ) );
	} ), ep );

}

}
}
