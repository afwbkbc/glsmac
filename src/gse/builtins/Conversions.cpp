#include "Conversions.h"

#include "gse/GSE.h"
#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/value/Undefined.h"
#include "gse/value/Int.h"
#include "gse/value/Float.h"
#include "gse/value/String.h"

#include "types/Color.h"

namespace gse {
namespace builtins {

void Conversions::AddToContext( context::Context* ctx, ExecutionPointer& ep ) {

#define CONVERSION_ERROR( _type ) throw Exception( EC.CONVERSION_ERROR, "Could not convert " + v->GetTypeString() + " to " + _type + ": " + v->ToString(), GSE_CALL );

#define CONVERT_COLOR( _type, _constructor, _min, _max ) { \
	N_GETVALUE( r, 0, _type ); \
    if ( r < _min || r > _max ) throw Exception( EC.INVALID_CALL, "Red value should be between " + std::to_string( _min ) + " and " + std::to_string( _max ) + ": " + std::to_string( r ), GSE_CALL ); \
	N_GETVALUE( g, 1, _type ); \
    if ( g < _min || g > _max ) throw Exception( EC.INVALID_CALL, "Green value should be between " + std::to_string( _min ) + " and " + std::to_string( _max ) + ": " + std::to_string( g ), GSE_CALL ); \
	N_GETVALUE( b, 2, _type ); \
    if ( b < _min || b > _max ) throw Exception( EC.INVALID_CALL, "Blue value should be between " + std::to_string( _min ) + " and " + std::to_string( _max ) + ": " + std::to_string( b ), GSE_CALL ); \
	if ( arguments.size() == 4 ) { \
		N_GETVALUE( a, 3, _type ); \
	    if ( a < _min || a > _max ) throw Exception( EC.INVALID_CALL, "Alpha value should be between " + std::to_string( _min ) + " and " + std::to_string( _max ) + ": " + std::to_string( a ), GSE_CALL ); \
		return _constructor( r, g, b, a ).Wrap(); \
	} \
	return _constructor( r, g, b ).Wrap(); \
}

	ctx->CreateBuiltin( "to_string", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GET( v, 0 );
		return VALUE( value::String, v->ToString() );
	} ), ep );

	ctx->CreateBuiltin( "to_dump", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GET( v, 0 );
		return VALUE( value::String, v->Dump() );
	} ), ep );

	ctx->CreateBuiltin( "to_int", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		int64_t value = 0;
		switch ( v->type ) {
			case Value::T_INT: {
				value = ( (value::Int*)v )->value;
				break;
			}
			case Value::T_STRING: {
				try {
					value = std::stol( ((value::String*)v)->value );
				}
				catch ( std::logic_error const& ex ) {
					CONVERSION_ERROR( "Int" )
				}
				break;
			}
			default:
				CONVERSION_ERROR( "Int" );
		}
		return VALUE( value::Int, value );
	} ), ep );

	ctx->CreateBuiltin( "to_float", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		float value = 0.0f;
		switch ( v->type ) {
			case Value::T_FLOAT: {
				value = ( (value::Float*)v)->value;
				break;
			}
			case Value::T_INT: {
				value = ( (value::Int*)v)->value;
				break;
			}
			case Value::T_STRING: {
				try {
					value = std::stof( ((value::String*)v)->value );
				}
				catch ( std::logic_error const& ex ) {
					CONVERSION_ERROR( "Float" )
				}
				break;
			}
			default:
				CONVERSION_ERROR( "Float" );
		}
		return VALUE( value::Float, value );
	} ), ep );

	ctx->CreateBuiltin( "to_color", NATIVE_CALL() {
		N_EXPECT_ARGS_MIN_MAX( 3, 4 );
		const auto f_err = [ &ctx, &si, &ep ] () {
			throw Exception( EC.INVALID_CALL, "Color can be specified either by floats (0.0 to 1.0) or by ints (0 to 255)", GSE_CALL );
		};

		switch ( arguments.at( 0 )->type ) {
			case Value::T_FLOAT: CONVERT_COLOR( Float, types::Color, 0.0f, 1.0f );
			case Value::T_INT: CONVERT_COLOR( Int, types::Color::FromRGBA, 0, 255 );
			default:
				f_err();
		}
		return VALUE( value::Undefined );
	} ), ep );

#undef CONVERT_COLOR

#undef CONVERSION_ERROR
}

}
}
