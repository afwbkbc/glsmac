#include "Interpreter.h"

#include "gse/program/Value.h"
#include "gse/program/Variable.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"

#include "gse/type/Type.h"
#include "gse/type/Undefined.h"
#include "gse/type/Bool.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"
#include "gse/type/ObjectRef.h"
#include "gse/type/Callable.h"

namespace gse {

using namespace program;
using namespace type;

namespace runner {

void Interpreter::Execute( Context* ctx, const program::Program* program ) const {
	ExecuteScope( ctx, program->body );
}

void Interpreter::ExecuteScope( Context* ctx, const program::Scope* scope ) const {
	for ( const auto& it : scope->body ) {
		ExecuteStatement( ctx, it );
	}
}

void Interpreter::ExecuteStatement( Context* ctx, const program::Statement* statement ) const {
	EvaluateExpression( ctx, statement->body );
}

const gse::Value Interpreter::EvaluateExpression( Context* ctx, const program::Expression* expression ) const {
	if ( !expression->op ) {
		ASSERT( !expression->b, "expression has second operand but no operator" );
		ASSERT( expression->a, "expression is empty" );
		return EvaluateOperand( ctx, expression->a );
	}
	switch ( expression->op->op ) {
		//OT_RETURN,
		case Operator::OT_ASSIGN: {
			ASSERT( expression->a, "missing assignment target" );
			const auto result = EvaluateOperand( ctx, expression->b );
			//const auto* var = EvaluateVariable( ctx, expression->a );
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto* var = (Variable*)expression->a;
					if ( var->hints & Variable::VH_CREATE ) {
						ctx->CreateVariable( var->name, result );
					}
					else {
						ctx->UpdateVariable( var->name, result );
					}
					break;
				}
				case Operand::OT_EXPRESSION: {
					// property of object?
					const auto target = EvaluateExpression( ctx, (Expression*)expression->a );
					// assign to reference
					WriteByRef( target, result );
					break;
				}
				default: {
					ASSERT( false, "unexpected assignment target: " + expression->a->ToString() );
				}
			}
			return result;
		}
			/*OT_NOT,*/
		case Operator::OT_EQ: {
			return VALUE( Bool,
				Deref( EvaluateOperand( ctx, expression->a ) ) ==
					Deref( EvaluateOperand( ctx, expression->b ) )
			);
		}
			/*OT_NE,
			OT_LT,
			OT_LTE,
			OT_GT,
			OT_GTE,
			OT_AND,
			OT_OR,*/
#define MATH_OP_BEGIN( _op ) \
            const auto av = Deref( EvaluateOperand( ctx, expression->a ) ); \
            const auto bv = Deref( EvaluateOperand( ctx, expression->b ) ); \
            const auto* a = av.Get(); \
            const auto* b = bv.Get(); \
            ASSERT( a->type == b->type, "operands have different types" ); \
            switch ( a->type ) { \
                case Type::T_INT: \
                    return VALUE( Int, ( (Int*)a )->value _op ( (Int*)b )->value );
#define MATH_OP_BEGIN_F( _op ) \
        MATH_OP_BEGIN( _op ) \
        case Type::T_FLOAT: \
                return VALUE( Float, ( (Float*)a )->value _op ( (Float*)b )->value );
#define MATH_OP_END() \
                default: \
                    ASSERT( false, "operation not supported for operands of type: " + std::to_string( a->type ) ); \
            }
#define MATH_OP( _op ) \
        MATH_OP_BEGIN_F( _op ) \
        MATH_OP_END()
		case Operator::OT_ADD: {
			MATH_OP_BEGIN_F( + )
				case Type::T_STRING:
					return VALUE( String, ( (String*)a )->value + ( (String*)b )->value );
			MATH_OP_END()
		}
		case Operator::OT_SUB: {
			MATH_OP( - )
		}
		case Operator::OT_MULT: {
			MATH_OP( * )
		}
		case Operator::OT_DIV: {
			MATH_OP( / )
		}
		case Operator::OT_MOD: {
			MATH_OP_BEGIN( % )
			MATH_OP_END()
		}
#undef MATH_OP
#undef MATH_OP_BEGIN
#undef MATH_OP_BEGIN_F
#undef MATH_OP_END
#define MATH_OP( _op ) \
        if ( expression->a ) { \
            ASSERT( !expression->b, "only one operand required, found two" ); \
            const auto varname = EvaluateVarName( ctx, expression->a ); \
            const auto value = ctx->GetVariable( varname ); \
            ASSERT( value.Get()->type == Type::T_INT, "expected int, found " + value.ToString() ); \
            ctx->UpdateVariable( varname, VALUE( Int, ( (Int*)value.Get() )->value _op 1 ) ); \
            return value; \
        } \
        else if ( expression->b ) { \
            const auto varname = EvaluateVarName( ctx, expression->b ); \
            const auto value = ctx->GetVariable( varname ); \
            ASSERT( value.Get()->type == Type::T_INT, "expected int, found " + value.ToString() ); \
            const auto result = VALUE( Int, ( (Int*)value.Get() )->value _op 1 ); \
            ctx->UpdateVariable( varname, result ); \
            return result; \
        } \
        else { \
            ASSERT( false, "operands not found" ); \
        }
		case Operator::OT_INC: {
			MATH_OP( + )
		}
		case Operator::OT_DEC: {
			MATH_OP( - )
		}
#undef MATH_OP
#define MATH_OP_BEGIN( _op ) \
        const auto varname = EvaluateVarName( ctx, expression->a ); \
        const auto av = ctx->GetVariable( varname ); \
        const auto* a = av.Get(); \
        const auto bv = EvaluateOperand( ctx, expression->b ); \
        const auto* b = bv.Get(); \
        ASSERT( a->type == b->type, "operands have different types" ); \
        gse::Value result = VALUE( Undefined ); \
        switch ( a->type ) { \
            case Type::T_INT: { \
                result = VALUE( Int, ( (Int*)a )->value _op ( (Int*)b )->value ); \
                break; \
            }
#define MATH_OP_BEGIN_F( _op ) \
        MATH_OP_BEGIN( _op ) \
            case Type::T_FLOAT: { \
                result = VALUE( Float, ( (Float*)a )->value _op ( (Float*)b )->value ); \
                break; \
            }
#define MATH_OP_END() \
            default: \
                ASSERT( false, "operation not supported for operands of type: " + std::to_string( a->type ) ); \
        } \
        ctx->UpdateVariable( varname, result ); \
        return result;
#define MATH_OP( _op ) \
        MATH_OP_BEGIN_F( _op ) \
        MATH_OP_END()
		case Operator::OT_INC_BY: {
			MATH_OP_BEGIN_F( + )
				case Type::T_STRING: {
					result = VALUE( String, ( (String*)a )->value + ( (String*)b )->value );
					break;
				}
			MATH_OP_END()
		}
		case Operator::OT_DEC_BY: {
			MATH_OP( - )
		}
		case Operator::OT_MULT_BY: {
			MATH_OP( * )
		}
		case Operator::OT_DIV_BY: {
			MATH_OP( / )
		}
		case Operator::OT_MOD_BY: {
			MATH_OP_BEGIN( % )
			MATH_OP_END()
		}
#undef MATH_OP
#undef MATH_OP_BEGIN
#undef MATH_OP_BEGIN_F
#undef MATH_OP_END
		case Operator::OT_CHILD: {
			ASSERT( expression->a, "parent object expected" );
			const auto childname = EvaluateVarName( ctx, expression->b );
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto objv = ctx->GetVariable( ( (Variable*)expression->a )->name );
					const auto* obj = objv.Get();
					ASSERT( obj->type == Type::T_OBJECT, "parent is not object: " + obj->ToString() );
					return ( (type::Object*)obj )->GetRef( childname );
				}
				case Operand::OT_EXPRESSION: {
					const auto refv = EvaluateExpression( ctx, (Expression*)expression->a );
					ASSERT( refv.Get()->type == Type::T_OBJECTREF, "parent is not reference: " + refv.ToString() );
					const auto* ref = (ObjectRef*)refv.Get();
					const auto objv = ref->object->Get( ref->key );
					ASSERT( objv.Get()->type == Type::T_OBJECT, "parent is not object: " + objv.ToString() );
					return ( (type::Object*)objv.Get() )->GetRef( childname );
				}
				default: {
					ASSERT( false, "parent is not object: " + expression->a->ToString() );
				}
			}
		}
		default: {
			ASSERT( false, "operator " + expression->op->ToString() + " not implemented" );
		}
	}
}

const gse::Value Interpreter::EvaluateOperand( Context* ctx, const program::Operand* operand ) const {
	ASSERT( operand, "operand is null" );
	switch ( operand->type ) {
		case Operand::OT_VALUE: {
			return ( (program::Value*)operand )->value;
		}
		case Operand::OT_VARIABLE: {
			return ctx->GetVariable( ( (program::Variable*)operand )->name );
		}
		case Operand::OT_OBJECT: {
			type::Object::properties_t properties = {};
			const auto* obj = (program::Object*)operand;
			for ( const auto& it : obj->properties ) {
				properties.insert_or_assign( it.first, EvaluateExpression( ctx, it.second ) );
			}
			return VALUE( type::Object, properties );
		}
		case Operand::OT_SCOPE: {
			ctx->PushScope();
			ExecuteScope( ctx, (program::Scope*)operand );
			ctx->PopScope();
			return VALUE( Undefined );
		}
		case Operand::OT_EXPRESSION: {
			return EvaluateExpression( ctx, ( (program::Expression*)operand ) );
		}
		case Operand::OT_FUNCTION: {
			const auto* func = (program::Function*)operand;
			return VALUE( Function, new program::Program( func->body ) );
		}
		case Operand::OT_CALL: {
			const auto* call = (program::Call*)operand;
			const auto callable = Deref( EvaluateExpression( ctx, call->callable ) );
			switch ( callable.Get()->type ) {
				case Type::T_CALLABLE: {
					Callable::function_arguments_t arguments = {};
					for ( const auto& it : call->arguments ) {
						arguments.push_back( Deref( EvaluateExpression( ctx, it ) ) );
					}
					return ( (Callable*)callable.Get() )->Run( nullptr, arguments );
				}
				default:
					ASSERT( false, "callable expected, found: " + callable.ToString() );
			}
		}
		default: {
			ASSERT( false, "operand " + operand->ToString() + " not implemented" );
		}
	}
}

const std::string& Interpreter::EvaluateString( Context* ctx, const program::Operand* operand ) const {
	const auto result = EvaluateOperand( ctx, operand );
	ASSERT( result.Get()->type == Type::T_STRING, "expected string, found " + result.ToString() );
	return ( (String*)result.Get() )->value;
}

const program::Variable* Interpreter::EvaluateVariable( Context* ctx, const program::Operand* operand ) const {
	ASSERT( operand->type == Operand::OT_VARIABLE, "expected variable, found " + operand->ToString() );
	return (Variable*)operand;
}

const std::string Interpreter::EvaluateVarName( Context* ctx, const program::Operand* operand ) const {
	const auto* var = EvaluateVariable( ctx, operand );
	ASSERT( var->hints == Variable::VH_NONE, "unexpected variable hints" );
	return var->name;
}

const gse::Value& Interpreter::Deref( const gse::Value& value ) const {
	switch ( value.Get()->type ) {
		case Type::T_OBJECTREF: {
			const auto* ref = (ObjectRef*)value.Get();
			return ref->object->Get( ref->key );
		}
		default:
			return value;
	}
}

void Interpreter::WriteByRef( const gse::Value& ref, const gse::Value& value ) const {
	ASSERT_NOLOG( ref.Get()->type == Type::T_OBJECTREF, "reference expected, found " + ref.ToString() );
	const auto* r = (ObjectRef*)ref.Get();
	r->object->Set( r->key, value );
}

Interpreter::Function::Function( const program::Program* program )
	: program( program ) {
	// nothing
}

gse::Value Interpreter::Function::Run( GSE* gse, const Callable::function_arguments_t arguments ) {
	ASSERT_NOLOG( false, "functions not implemented" );
}

}
}
