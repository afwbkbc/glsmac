#include "Interpreter.h"

#include "gse/program/Value.h"
#include "gse/program/Variable.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"
#include "gse/program/Array.h"
#include "gse/program/If.h"
#include "gse/program/ElseIf.h"
#include "gse/program/Else.h"

#include "gse/type/Type.h"
#include "gse/type/Undefined.h"
#include "gse/type/Bool.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/String.h"
#include "gse/type/Array.h"
#include "gse/type/Object.h"
#include "gse/type/ArrayRef.h"
#include "gse/type/ArrayRangeRef.h"
#include "gse/type/ObjectRef.h"
#include "gse/type/Callable.h"
#include "gse/type/Range.h"

namespace gse {

using namespace program;
using namespace type;

namespace runner {

const gse::Value Interpreter::Execute( Context* ctx, const program::Program* program ) const {
	return ExecuteScope( ctx, program->body );
}

const gse::Value Interpreter::ExecuteScope( Context* ctx, const program::Scope* scope ) const {
	gse::Value result = VALUE( Undefined );
	for ( const auto& it : scope->body ) {
		switch ( it->control_type ) {
			case program::Control::CT_STATEMENT: {
				result = ExecuteStatement( ctx, (program::Statement*)it );
				break;
			}
			case program::Control::CT_CONDITIONAL: {
				result = ExecuteConditional( ctx, (program::Conditional*)it );
				break;
			}
			default:
				ASSERT( false, "unexpected control type: " + it->ToString() );
		}
		if ( result.Get()->type != Type::T_UNDEFINED ) {
			// got return statement
			break;
		}
	}
	return result;
}

const gse::Value Interpreter::ExecuteStatement( Context* ctx, const program::Statement* statement ) const {
	bool returnflag = false;
	const auto result = EvaluateExpression( ctx, statement->body, &returnflag );
	if ( returnflag ) {
		return result;
	}
	return VALUE( Undefined );
}

const gse::Value Interpreter::ExecuteConditional( Context* ctx, const program::Conditional* conditional, bool is_nested ) const {
	switch ( conditional->conditional_type ) {
		case program::Conditional::CT_IF: {
			const auto* c = (program::If*)conditional;
			const auto result = EvaluateExpression( ctx, c->condition );
			ASSERT( result.Get()->type == Type::T_BOOL, "result of expression is not bool" );
			if ( ( (Bool*)result.Get() )->value ) {
				return ExecuteScope( ctx, c->body );
			}
			else if ( c->els ) {
				return ExecuteConditional( ctx, c->els, true );
			}
			else {
				return VALUE( type::Undefined );
			}
		}
		case program::Conditional::CT_ELSEIF: {
			ASSERT( is_nested, "elseif without if" );
			const auto* c = (program::ElseIf*)conditional;
			const auto result = EvaluateExpression( ctx, c->condition );
			ASSERT( result.Get()->type == Type::T_BOOL, "result of expression is not bool" );
			if ( ( (Bool*)result.Get() )->value ) {
				return ExecuteScope( ctx, c->body );
			}
			else if ( c->els ) {
				return ExecuteConditional( ctx, c->els, true );
			}
			else {
				return VALUE( type::Undefined );
			}
		}
		case program::Conditional::CT_ELSE: {
			ASSERT( is_nested, "else without if" );
			const auto* c = (program::Else*)conditional;
			return ExecuteScope( ctx, c->body );
		}
		default:
			ASSERT( false, "unexpected conditional type: " + conditional->ToString() );
	}
}

const gse::Value Interpreter::EvaluateExpression( Context* ctx, const program::Expression* expression, bool* returnflag ) const {
	if ( !expression->op ) {
		ASSERT( !expression->b, "expression has second operand but no operator" );
		ASSERT( expression->a, "expression is empty" );
		return EvaluateOperand( ctx, expression->a );
	}
	switch ( expression->op->op ) {
		case Operator::OT_RETURN: {
			ASSERT( returnflag, "return keyword not allowed here" );
			ASSERT( !*returnflag, "already returning" );
			ASSERT( !expression->a, "unexpected left operand before return" );
			ASSERT( expression->b, "return value or expression expected" );
			*returnflag = true;
			return Deref( EvaluateOperand( ctx, expression->b ) );
		}
		case Operator::OT_ASSIGN: {
			ASSERT( expression->a, "missing assignment target" );
			auto result = Deref( EvaluateOperand( ctx, expression->b ) ).Clone(); // for now always copy on assignment
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
		case Operator::OT_NOT: {
			ASSERT( !expression->a, "unary not may not have left operand" );
			return VALUE( Bool, !EvaluateBool( ctx, expression->b ) );
		}
#define CMP_OP( _op ) { \
        return VALUE( Bool, \
            Deref( EvaluateOperand( ctx, expression->a ) ) _op \
                Deref( EvaluateOperand( ctx, expression->b ) ) \
            ); \
}
		case Operator::OT_EQ: CMP_OP( == )
		case Operator::OT_NE: CMP_OP( != )
		case Operator::OT_LT: CMP_OP( < )
		case Operator::OT_LTE: CMP_OP( <= )
		case Operator::OT_GT: CMP_OP( > )
		case Operator::OT_GTE: CMP_OP( >= )
#undef CMP_OP
#define CMP_BOOL( _op ) { \
        return VALUE( Bool, \
            EvaluateBool( ctx, expression->a ) _op \
                EvaluateBool( ctx, expression->b ) \
            ); \
}
		case Operator::OT_AND: CMP_BOOL( && )
		case Operator::OT_OR: CMP_BOOL( || )
#undef CMP_BOOL
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
				case Type::T_ARRAY: {
					type::Array::elements_t elements = ( (type::Array*)a )->value;
					elements.insert( elements.end(), ( (type::Array*)b )->value.begin(), ( (type::Array*)b )->value.end() );
					return VALUE( type::Array, elements );
				}
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
				case Type::T_ARRAY: {
					type::Array::elements_t elements = ( (type::Array*)a )->value;
					elements.insert( elements.end(), ( (type::Array*)b )->value.begin(), ( (type::Array*)b )->value.end() );
					result = VALUE( type::Array, elements );
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
		case Operator::OT_AT: {
			ASSERT( expression->a, "parent array expected" );

			std::optional< size_t > index, from, to;
			const auto valv = EvaluateRange( ctx, expression->b );
			const auto* val = valv.Get();
			switch ( val->type ) {
				case Type::T_INT: {
					index = ( (type::Int*)val )->value;
					from = std::nullopt;
					to = std::nullopt;
					break;
				}
				case Type::T_RANGE: {
					const auto* range = (type::Range*)val;
					index = std::nullopt;
					from = range->from;
					to = range->to;
					break;
				}
				default:
					ASSERT( false, "unexpected index type: " + val->ToString() );
			}
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto arrv = ctx->GetVariable( ( (Variable*)expression->a )->name );
					const auto* arr = arrv.Get();
					ASSERT( arr->type == Type::T_ARRAY, "parent is not array: " + arr->ToString() );
					if ( index.has_value() ) {
						return ( (type::Array*)arr )->GetRef( index.value() );
					}
					else {
						return ( (type::Array*)arr )->GetRangeRef( from, to );
					}
				}
				case Operand::OT_EXPRESSION: {
					const auto refv = EvaluateExpression( ctx, (Expression*)expression->a );
					const auto* ref = refv.Get();
					switch ( ref->type ) {
						case Type::T_ARRAYREF: {
							const auto* r = (ArrayRef*)refv.Get();
							const auto arrv = r->array->Get( r->index );
							const auto* arr = arrv.Get();
							ASSERT( arr->type == Type::T_ARRAY, "parent is not array: " + arr->ToString() );
							if ( index.has_value() ) {
								return ( (type::Array*)arr )->GetRef( index.value() );
							}
							else {
								return ( (type::Array*)arr )->GetRangeRef( from, to );
							}
						}
						case Type::T_ARRAYRANGEREF: {
							ASSERT( false, "TODO: T_ARRAYRANGEREF" );
						}
						default:
							ASSERT( false, "unexpected expression result: " + ref->ToString() );
					}
				}
				default: {
					ASSERT( false, "parent is not array: " + expression->a->ToString() );
				}
			}
		}
		case Operator::OT_APPEND: {
			const auto arrv = ctx->GetVariable( ( (program::Variable*)expression->a )->name );
			const auto* arr = arrv.Get();
			ASSERT( arr->type == Type::T_ARRAY, "left operand is not array" );
			const auto value = EvaluateOperand( ctx, expression->b );
			( (type::Array*)arr )->Append( value );
			return value;
		}
		case Operator::OT_RANGE: {

			std::optional< size_t > from = std::nullopt;
			std::optional< size_t > to = std::nullopt;

			if ( expression->a ) {
				const auto fromv = EvaluateRange( ctx, expression->a, true );
				const auto* fromr = fromv.Get();
				ASSERT( fromr->type == Type::T_INT, "int expected here" );
				from = ( (type::Int*)fromr )->value;
			}
			if ( expression->b ) {
				const auto tov = EvaluateRange( ctx, expression->b, true );
				const auto* tor = tov.Get();
				ASSERT( tor->type == Type::T_INT, "int expected here" );
				to = ( (type::Int*)tor )->value;
			}

			return VALUE( Range, from, to );
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
		case Operand::OT_ARRAY: {
			auto* arr = (program::Array*)operand;
			type::Array::elements_t elements;
			for ( const auto& it : arr->elements ) {
				elements.push_back( EvaluateExpression( ctx, it ) );
			}
			return VALUE( type::Array, elements );
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
			std::vector< std::string > parameters = {};
			for ( const auto& it : func->parameters ) {
				ASSERT( it->hints == Variable::VH_NONE, "function parameters can't have modifiers" );
				parameters.push_back( it->name );
			}
			return VALUE( Function, this, ctx, parameters, new program::Program( func->body ) );
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

const std::string Interpreter::EvaluateString( Context* ctx, const program::Operand* operand ) const {
	const auto result = Deref( EvaluateOperand( ctx, operand ) );
	ASSERT( result.Get()->type == Type::T_STRING, "expected string, found " + result.ToString() );
	return ( (String*)result.Get() )->value;
}

const gse::Value Interpreter::EvaluateRange( Context* ctx, const program::Operand* operand, const bool only_index ) const {
	ASSERT( operand, "index operand missing" );
	const auto get_index = []( const gse::Value& value ) -> size_t {
		const auto* val = value.Get();
		ASSERT_NOLOG( val->type == Type::T_INT, "index must be integer" );
		ASSERT_NOLOG( ( (type::Int*)val )->value >= 0, "index must not be negative" );
		return ( (type::Int*)val )->value;
	};
	switch ( operand->type ) {
		case Operand::OT_VALUE: {
			return VALUE( type::Int, get_index( ( (program::Value*)operand )->value ) );
		}
		case Operand::OT_VARIABLE: {
			return VALUE( type::Int, get_index( ctx->GetVariable( ( (Variable*)operand )->name ) ) );
		}
		case Operand::OT_EXPRESSION: {
			const auto result = Deref( EvaluateExpression( ctx, (Expression*)operand ) );
			switch ( result.Get()->type ) {
				case Type::T_INT: {
					return VALUE( type::Int, get_index( result ) );
				}
				case Type::T_RANGE: {
					ASSERT( !only_index, "range not allowed here" );
					const auto* range = (type::Range*)result.Get();
					return VALUE( type::Range, range->from, range->to );
				}
				default:
					ASSERT( false, "unexpected index expression result type: " + result.ToString() );
			}
		}
		default: {
			ASSERT( false, "unexpected index type: " + operand->ToString() );
		}
	}
}

const bool Interpreter::EvaluateBool( Context* ctx, const program::Operand* operand ) const {
	const auto result = Deref( EvaluateOperand( ctx, operand ) );
	ASSERT( result.Get()->type == Type::T_BOOL, "expected bool, found " + result.ToString() );
	return ( (Bool*)result.Get() )->value;
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

const gse::Value Interpreter::Deref( const gse::Value& value ) const {
	switch ( value.Get()->type ) {
		case Type::T_ARRAYREF: {
			const auto* ref = (ArrayRef*)value.Get();
			return ref->array->Get( ref->index );
		}
		case Type::T_ARRAYRANGEREF: {
			const auto* ref = (ArrayRangeRef*)value.Get();
			return ref->array->GetSubArray( ref->from, ref->to );
		}
		case Type::T_OBJECTREF: {
			const auto* ref = (ObjectRef*)value.Get();
			return ref->object->Get( ref->key );
		}
		default:
			return value;
	}
}

void Interpreter::WriteByRef( const gse::Value& ref, const gse::Value& value ) const {
	switch ( ref.Get()->type ) {
		case Type::T_OBJECTREF: {
			const auto* r = (ObjectRef*)ref.Get();
			r->object->Set( r->key, value );
			break;
		}
		case Type::T_ARRAYREF: {
			const auto* r = (ArrayRef*)ref.Get();
			r->array->Set( r->index, value );
			break;
		}
		case Type::T_ARRAYRANGEREF: {
			const auto* r = (ArrayRangeRef*)ref.Get();
			r->array->SetSubArray( r->from, r->to, value );
			break;
		}
		default:
			ASSERT_NOLOG( false, "reference expected, found " + ref.ToString() );
	}
}

Interpreter::Function::Function(
	const Interpreter* runner,
	Context const* parent_context,
	const std::vector< std::string >& parameters,
	const program::Program* const program
)
	: runner( runner )
	, parent_context( parent_context )
	, parameters( parameters )
	, program( program ) {
	// nothing
}

gse::Value Interpreter::Function::Run( GSE* gse, const Callable::function_arguments_t& arguments ) {
	auto* ctx = parent_context->CreateFunctionScope( parameters, arguments );
	const auto result = runner->Execute( ctx, program );
	delete ctx;
	return result;
}

}
}
