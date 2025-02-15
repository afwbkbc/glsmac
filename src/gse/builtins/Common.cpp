#include "Common.h"

#include "gse/GSE.h"
#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/type/Array.h"
#include "gse/type/Bool.h"
#include "gse/type/Undefined.h"
#include "gse/type/Object.h"

namespace gse {
namespace builtins {

void Common::AddToContext( context::Context* ctx, ExecutionPointer& ep ) {

	ctx->CreateBuiltin( "typeof", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		return VALUE( type::String, type::Type::GetTypeString( v->type ) );
	} ), ep );

	ctx->CreateBuiltin( "classof", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		if ( v->type == type::Type::T_OBJECT ) {
			return VALUE( type::String, ( ( type::Object*)v )->object_class );
		}
		return VALUE( type::Undefined );
	} ), ep );

	ctx->CreateBuiltin( "sizeof", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		size_t size = 0;
		switch ( v->type ) {
			case type::Type::T_ARRAY: {
				size = ((type::Array*)v)->value.size();
				break;
			}
			default:
				throw Exception( EC.OPERATION_NOT_SUPPORTED, "Could not get size of " + v->GetTypeString( v->type ) + ": " + v->ToString(), GSE_CALL );
		}
		return VALUE( type::Int, size );
	} ), ep );

	ctx->CreateBuiltin("is_defined", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		switch ( v->type ) {
			case type::Type::T_NOTHING:
			case type::Type::T_UNDEFINED:
				return VALUE( gse::type::Bool, false );
			default:
				return VALUE( gse::type::Bool, true );
		}
	} ), ep );

	ctx->CreateBuiltin( "is_empty", NATIVE_CALL() {
		N_EXPECT_ARGS( 1 );
		N_GETPTR( v, 0 );
		bool is_empty = true;
		switch ( v->type ) {
			case type::Type::T_STRING: {
				is_empty = ((type::String*)v)->value.empty();
				break;
			}
			case type::Type::T_ARRAY: {
				is_empty = ((type::Array*)v)->value.empty();
				break;
			}
			case type::Type::T_OBJECT: {
				is_empty = ((type::Object*)v)->value.empty();
				break;
			}
			default:
				throw Exception( EC.OPERATION_NOT_SUPPORTED, "Could not get size of " + v->GetTypeString( v->type ) + ": " + v->ToString(), GSE_CALL );
		}
		return VALUE( type::Bool, is_empty );
	} ), ep );

	ctx->CreateBuiltin( "clone", NATIVE_CALL()
	{
		N_EXPECT_ARGS( 1 );
		const auto& v = arguments.at(0);
		switch ( v.Get()->type ) {
			case type::Type::T_OBJECT:
			case type::Type::T_ARRAY:
				return v.Clone();
			default:
				throw Exception( EC.OPERATION_NOT_SUPPORTED, "Cloning of type " + v.GetTypeString() + " is not supported", GSE_CALL );
		}
	} ), ep );

	ctx->CreateBuiltin( "undefined", VALUE( type::Undefined ), ep );
}

}
}
