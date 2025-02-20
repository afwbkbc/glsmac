#include "Math.h"

#include <cmath>

#include "gse/GSE.h"
#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/value/Int.h"
#include "gse/value/Float.h"
#include "gse/value/Undefined.h"

namespace gse {
namespace builtins {

void Math::AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) {

#define AB( _name, _f_int, _f_float ) \
	ctx->CreateBuiltin( _name, NATIVE_CALL() { \
		N_EXPECT_ARGS( 2 ); \
		for ( uint8_t i = 0 ; i < 2 ; i++ ) { \
			const auto& value = arguments.at(i); \
			if ( value->type != Value::T_INT && value->type != Value::T_FLOAT ) { \
				GSE_ERROR( EC.INVALID_CALL, "Argument " + std::to_string( i ) + " is expected to be " + Value::GetTypeStringStatic( Value::T_INT ) + " or " + Value::GetTypeStringStatic( Value::T_FLOAT ) + ", found: " + value->GetTypeString() ); \
			} \
		} \
		const auto& a = arguments.at(0); \
		const auto& b = arguments.at(1); \
		if ( a->type != b->type ) { \
			GSE_ERROR( EC.INVALID_CALL, "Arguments are of different types: " + a->GetTypeString() + ", " + b->GetTypeString() ); \
		} \
		switch ( a->type ) { \
			case Value::T_INT: \
				return VALUE( value::Int,, _f_int( ((value::Int*)a)->value, ((value::Int*)b)->value ) ); \
			case Value::T_FLOAT: \
				return VALUE( value::Float,, _f_float( ((value::Float*)a)->value, ((value::Float*)b)->value ) ); \
			default: \
				THROW( "unexpected type: " + a->GetTypeString() ); \
		} \
	} ), gc_space, ep );
	AB( "min", std::min, std::fmin )
	AB( "max", std::max, std::fmax )
#undef AB

#define F( _func, _in_type, _out_type ) \
	ctx->CreateBuiltin( #_func, NATIVE_CALL() { \
		N_EXPECT_ARGS( 1 ); \
		N_GETVALUE( v, 0, _in_type ); \
		return VALUE( value::_out_type,, std::_func( v ) ); \
	} ), gc_space, ep );
	F( floor, Float, Int )
	F( round, Float, Int )
	F( ceil, Float, Int )
	F( sqrt, Float, Float )
#undef F

}

}
}
