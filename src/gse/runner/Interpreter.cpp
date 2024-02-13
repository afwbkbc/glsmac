#include "Interpreter.h"

#include "gse/program/Value.h"
#include "gse/program/Variable.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"
#include "gse/program/Array.h"
#include "gse/program/If.h"
#include "gse/program/ElseIf.h"
#include "gse/program/Else.h"
#include "gse/program/While.h"
#include "gse/program/Try.h"
#include "gse/program/Catch.h"

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
#include "gse/type/Exception.h"

#include "util/String.h"

namespace gse {

using namespace program;
using namespace type;

namespace runner {

const gse::Value Interpreter::Execute( Context* ctx, const program::Program* program ) const {
	return EvaluateScope( ctx, program->body );
}

const gse::Value Interpreter::EvaluateScope( Context* ctx, const program::Scope* scope ) const {
	const auto subctx = ctx->ForkContext( ctx, scope->m_si, false );
	subctx->IncRefs();

	gse::Value result = VALUE( Undefined );
	for ( const auto& it : scope->body ) {
		switch ( it->control_type ) {
			case program::Control::CT_STATEMENT: {
				result = EvaluateStatement( subctx, (program::Statement*)it );
				break;
			}
			case program::Control::CT_CONDITIONAL: {
				result = EvaluateConditional( subctx, (program::Conditional*)it );
				break;
			}
			default:
				THROW( "unexpected control type: " + it->Dump() );
		}
		if ( result.Get()->type != Type::T_UNDEFINED ) {
			// got return statement
			break;
		}
	}

#ifdef DEBUG
	if ( m_are_scope_context_joins_enabled ) {
		subctx->JoinContext();
	}
#endif

	subctx->DecRefs();
	return result;
}

const gse::Value Interpreter::EvaluateStatement( Context* ctx, const program::Statement* statement ) const {
	bool returnflag = false;
	const auto result = EvaluateExpression( ctx, statement->body, &returnflag );
	if ( returnflag ) {
		return result;
	}
	return VALUE( Undefined );
}

const gse::Value Interpreter::EvaluateConditional( Context* ctx, const program::Conditional* conditional, bool is_nested ) const {
	switch ( conditional->conditional_type ) {
		case program::Conditional::CT_IF: {
			const auto* c = (program::If*)conditional;
			if ( EvaluateBool( ctx, c->condition ) ) {
				return EvaluateScope( ctx, c->body );
			}
			else if ( c->els ) {
				return EvaluateConditional( ctx, c->els, true );
			}
			else {
				return VALUE( type::Undefined );
			}
		}
		case program::Conditional::CT_ELSEIF: {
			if ( !is_nested ) {
				// TODO: move check to parser
				throw gse::Exception( EC.PARSE_ERROR, "Unexpected elseif without if", ctx, conditional->m_si );
			}
			const auto* c = (program::ElseIf*)conditional;
			if ( EvaluateBool( ctx, c->condition ) ) {
				return EvaluateScope( ctx, c->body );
			}
			else if ( c->els ) {
				return EvaluateConditional( ctx, c->els, true );
			}
			else {
				return VALUE( type::Undefined );
			}
		}
		case program::Conditional::CT_ELSE: {
			if ( !is_nested ) {
				// TODO: move check to parser
				throw gse::Exception( EC.PARSE_ERROR, "Unexpected else without if", ctx, conditional->m_si );
			}
			const auto* c = (program::Else*)conditional;
			return EvaluateScope( ctx, c->body );
		}
		case program::Conditional::CT_WHILE: {
			const auto* c = (program::While*)conditional;
			gse::Value result = VALUE( type::Undefined );
			while ( EvaluateBool( ctx, c->condition ) ) {
				result = EvaluateScope( ctx, c->body );
				if ( result.Get()->type != Type::T_UNDEFINED ) {
					break;
				}
			}
			return result;
		}
		case program::Conditional::CT_TRY: {
			const auto* c = (program::Try*)conditional;
			try {
				return EvaluateScope( ctx, c->body );
			}
			catch ( gse::Exception& e ) {
				const auto* h = c->handlers->handlers;
				auto it = h->properties.find( e.class_name );
				if ( it == h->properties.end() ) {
					it = h->properties.find( "" ); // check for default handler too
				}
				if ( it != h->properties.end() ) {
					const auto f = EvaluateExpression( ctx, it->second );
					if ( f.Get()->type != Type::T_CALLABLE ) {
						// TODO: check move to parser
						throw gse::Exception( EC.PARSE_ERROR, "Expected catch block, found: " + f.ToString(), ctx, it->second->m_si );
					}

					auto* func = (Function*)f.Get();
					const auto result = func->Run(
						ctx,
						it->second->m_si,
						{
							VALUE( type::Exception, e, e.GetBacktraceAndCleanup( ctx ) )
						}
					);
					return result;
				}
				else {
					throw e;
				}
			}
		}
		default:
			THROW( "unexpected conditional type: " + conditional->Dump() );
	}
}

const gse::Value Interpreter::EvaluateExpression( Context* ctx, const program::Expression* expression, bool* returnflag ) const {
	if ( !expression->op ) {
		ASSERT( !expression->b, "expression has second operand but no operator" );
		ASSERT( expression->a, "expression is empty" );
		return EvaluateOperand( ctx, expression->a );
	}
	const auto& operation_not_supported = [ &expression, &ctx ]( const std::string& a, const std::string& b ) -> gse::Exception {
		return gse::Exception( EC.OPERATION_NOT_SUPPORTED, "Operation " + expression->op->ToString() + " is not supported between " + a + " and " + b, ctx, expression->op->m_si );
	};
	switch ( expression->op->op ) {
		case Operator::OT_RETURN: {
			ASSERT( returnflag, "return keyword not allowed here" );
			ASSERT( !*returnflag, "already returning" );
			ASSERT( !expression->a, "unexpected left operand before return" );
			ASSERT( expression->b, "return value or expression expected" );
			*returnflag = true;
			return Deref( ctx, expression->b->m_si, EvaluateOperand( ctx, expression->b ) );
		}
		case Operator::OT_THROW: {
			ASSERT( !expression->a, "unexpected left operand before throw" );
			const auto& invalid_error_definition = [ &expression, &ctx ]() -> gse::Exception {
				return gse::Exception( EC.INVALID_CALL, "Invalid error definition. Expected: ErrorType(reason), found: " + expression->b->ToString(), ctx, expression->b->m_si );
			};
			if ( !expression->b || expression->b->type != Operand::OT_CALL ) {
				throw invalid_error_definition();
			}
			const auto* e = (program::Call*)expression->b; // it's not a call but it looks like a call
			if (
				!e->callable->a ||
					e->callable->a->type != Operand::OT_VARIABLE ||
					e->callable->op ||
					e->callable->b ||
					e->arguments.size() != 1 ) {
				throw invalid_error_definition();
			}
			const auto reason = EvaluateExpression( ctx, e->arguments[ 0 ] );
			if ( reason.Get()->type != Type::T_STRING ) {
				throw invalid_error_definition();
			}

			throw gse::Exception(
				( (program::Variable*)e->callable->a )->name,
				( (type::String*)reason.Get() )->value,
				ctx,
				expression->op->m_si
			);
		}
		case Operator::OT_ASSIGN: {
			ASSERT( expression->a, "missing assignment target" );
			auto result = Deref( ctx, expression->b->m_si, EvaluateOperand( ctx, expression->b ) ).Clone(); // for now always copy on assignment
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto* var = (Variable*)expression->a;
					if ( var->name[ 0 ] == '#' ) {
						throw gse::Exception( EC.INVALID_ASSIGNMENT, "Can't assign to builtin: " + var->name, ctx, var->m_si );
					}
					if ( var->hints & Variable::VH_CREATE_VAR ) {
						ctx->CreateVariable( var->name, result, &expression->a->m_si );
					}
					else if ( var->hints & Variable::VH_CREATE_CONST ) {
						ctx->CreateConst( var->name, result, &expression->a->m_si );
					}
					else {
						ctx->UpdateVariable( var->name, result, &expression->a->m_si );
					}
					break;
				}
				case Operand::OT_EXPRESSION: {
					// property of object?
					const auto target = EvaluateExpression( ctx, (Expression*)expression->a );
					// assign to reference
					WriteByRef( ctx, expression->a->m_si, target, result );
					break;
				}
				default:
					throw gse::Exception( EC.INVALID_ASSIGNMENT, (std::string)"Can't assign " + result.ToString() + " to " + expression->a->ToString(), ctx, expression->a->m_si );
			}
			return result;
		}
		case Operator::OT_NOT: {
			ASSERT( !expression->a, "unary not may not have left operand" );
			return VALUE( Bool, !EvaluateBool( ctx, expression->b ) );
		}
#define CMP_OP( _op ) { \
        return VALUE( Bool, \
            Deref( ctx, expression->a->m_si, EvaluateOperand( ctx, expression->a ) ) _op \
                Deref( ctx, expression->b->m_si, EvaluateOperand( ctx, expression->b ) ) \
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
            const auto av = Deref( ctx, expression->a->m_si, EvaluateOperand( ctx, expression->a ) ); \
            const auto bv = Deref( ctx, expression->b->m_si, EvaluateOperand( ctx, expression->b ) ); \
            const auto* a = av.Get(); \
            const auto* b = bv.Get(); \
            if ( a->type != b->type ) { \
                throw operation_not_supported( a->ToString(), b->ToString() ); \
            } \
            switch ( a->type ) { \
                case Type::T_INT: \
                    return VALUE( Int, ( (Int*)a )->value _op ( (Int*)b )->value );
#define MATH_OP_BEGIN_F( _op ) \
        MATH_OP_BEGIN( _op ) \
        case Type::T_FLOAT: \
                return VALUE( Float, ( (Float*)a )->value _op ( (Float*)b )->value );
#define MATH_OP_END() \
                default: \
                    throw operation_not_supported( a->ToString(), b->ToString() ); \
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
				case Type::T_OBJECT: {
					type::Object::properties_t properties = ( (type::Object*)a )->value;
					for ( const auto& it : ( (type::Object*)b )->value ) {
						if ( properties.find( it.first ) != properties.end() ) {
							throw gse::Exception( EC.OPERATION_FAILED, "Can't concatenate objects - duplicate key found: " + it.first, ctx, expression->op->m_si );
						}
						properties.insert_or_assign( it.first, it.second );
					}
					return VALUE( type::Object, properties );
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
            const auto value = ctx->GetVariable( varname, &expression->a->m_si ); \
            if ( value.Get()->type != Type::T_INT ) { \
                throw gse::Exception( EC.TYPE_ERROR, "Expected int, found: " + expression->a->ToString(), ctx, expression->a->m_si ); \
            } \
            ctx->UpdateVariable( varname, VALUE( Int, ( (Int*)value.Get() )->value _op 1 ), &expression->a->m_si ); \
            return value; \
        } \
        else if ( expression->b ) { \
            const auto varname = EvaluateVarName( ctx, expression->b ); \
            const auto value = ctx->GetVariable( varname, &expression->b->m_si ); \
            if ( value.Get()->type != Type::T_INT ) { \
                throw gse::Exception( EC.TYPE_ERROR, "Expected int, found: " + expression->b->ToString(), ctx, expression->b->m_si ); \
            } \
            const auto result = VALUE( Int, ( (Int*)value.Get() )->value _op 1 ); \
            ctx->UpdateVariable( varname, result, &expression->b->m_si ); \
            return result; \
        } \
        else { \
            THROW( "operands not found" ); \
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
        const auto av = Deref( ctx, expression->a->m_si, ctx->GetVariable( varname, &expression->a->m_si ) ); \
        const auto bv = Deref( ctx, expression->b->m_si, EvaluateOperand( ctx, expression->b ) ); \
        const auto* a = av.Get(); \
        const auto* b = bv.Get(); \
        if ( a->type != b->type ) {                                 \
            throw operation_not_supported( a->ToString(), b->ToString() ); \
        } \
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
            default:  \
                throw operation_not_supported( a->ToString(), b->ToString() ); \
        } \
        ctx->UpdateVariable( varname, result, &expression->a->m_si ); \
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
				case Type::T_OBJECT: {
					type::Object::properties_t properties = ( (type::Object*)a )->value;
					for ( const auto& it : ( (type::Object*)b )->value ) {
						if ( properties.find( it.first ) != properties.end() ) {
							throw gse::Exception( EC.OPERATION_FAILED, "Can't append object - duplicate key found: " + it.first, ctx, expression->op->m_si );
						}
						properties.insert_or_assign( it.first, it.second );
					}
					return VALUE( type::Object, properties );
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
			const auto& not_an_object = [ &ctx, &expression, &childname ]( const std::string& what, const si_t& si ) -> gse::Exception {
				return gse::Exception( EC.INVALID_DEREFERENCE, "Could not get ." + childname + " of non-object: " + what, ctx, expression->op->m_si );
			};
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto objv = ctx->GetVariable( ( (Variable*)expression->a )->name, &expression->a->m_si );
					const auto* obj = objv.Get();
					if ( obj->type != Type::T_OBJECT ) {
						throw not_an_object( obj->ToString(), expression->a->m_si );
					}
					return ( (type::Object*)obj )->GetRef( childname );
				}
				case Operand::OT_OBJECT: {
					const auto objv = EvaluateOperand( ctx, expression->a );
					const auto* obj = objv.Get();
					ASSERT( obj->type == Type::T_OBJECT, "parent is not object: " + obj->Dump() );
					return ( (type::Object*)obj )->Get( childname );
				}
				case Operand::OT_EXPRESSION: {
					const auto objv = Deref( ctx, expression->a->m_si, EvaluateExpression( ctx, (Expression*)expression->a ) );
					const auto* obj = objv.Get();
					if ( obj->type != Type::T_OBJECT ) {
						throw not_an_object( obj->ToString(), expression->a->m_si );
					}
					return ( (type::Object*)obj )->GetRef( childname );
				}
				default: {
					throw not_an_object( expression->a->ToString(), expression->a->m_si );
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
					THROW( "unexpected index type: " + val->ToString() );
			}
			const auto& not_an_array = [ &ctx, &index, &from, &to ]( const std::string& what, const si_t& si ) -> gse::Exception {
				return gse::Exception(
					EC.INVALID_DEREFERENCE, "Could not get " +
						( index.has_value()
							? "index " + std::to_string( index.value() )
							: "range [ " + std::to_string( from.value() ) + " : " + std::to_string( to.value() )
						) +
						" of non-array: " + what, ctx, si
				);
			};
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto arrv = ctx->GetVariable( ( (Variable*)expression->a )->name, &expression->a->m_si );
					const auto* arr = arrv.Get();
					if ( arr->type != Type::T_ARRAY ) {
						throw not_an_array( arr->ToString(), expression->a->m_si );
					}
					if ( index.has_value() ) {
						return ( (type::Array*)arr )->GetRef( index.value() );
					}
					else {
						return ( (type::Array*)arr )->GetRangeRef( from, to );
					}
				}
				case Operand::OT_ARRAY: {
					const auto arrv = EvaluateOperand( ctx, expression->a );
					const auto* arr = arrv.Get();
					ASSERT( arr->type == Type::T_ARRAY, "parent is not array: " + arr->Dump() );
					if ( index.has_value() ) {
						return ( (type::Array*)arr )->Get( index.value() );
					}
					else {
						ValidateRange( ctx, expression->b->m_si, (type::Array*)arr, from, to );
						return ( (type::Array*)arr )->GetSubArray( from, to );
					}
				}
				case Operand::OT_EXPRESSION: {
					const auto refv = EvaluateExpression( ctx, (Expression*)expression->a );
					const auto* ref = refv.Get();
					switch ( ref->type ) {
						case Type::T_ARRAY: {
							if ( index.has_value() ) {
								return ( (type::Array*)ref )->Get( index.value() );
							}
							else {
								return ( (type::Array*)ref )->GetRangeRef( from, to );
							}
						}
						case Type::T_ARRAYREF: {
							const auto* r = (ArrayRef*)ref;
							const auto arrv = r->array->Get( r->index );
							const auto* arr = arrv.Get();
							if ( arr->type != Type::T_ARRAY ) {
								throw not_an_array( arr->ToString(), expression->a->m_si );
							}
							if ( index.has_value() ) {
								return ( (type::Array*)arr )->GetRef( index.value() );
							}
							else {
								return ( (type::Array*)arr )->GetRangeRef( from, to );
							}
						}
						case Type::T_ARRAYRANGEREF: {
							THROW( "TODO: T_ARRAYRANGEREF" );
						}
						case Type::T_OBJECTREF: {
							const auto arrv = Deref( ctx, expression->a->m_si, refv );
							const auto* arr = arrv.Get();
							if ( arr->type != Type::T_ARRAY ) {
								throw not_an_array( arr->ToString(), expression->a->m_si );
							}
							if ( index.has_value() ) {
								return ( (type::Array*)arr )->GetRef( index.value() );
							}
							else {
								return ( (type::Array*)arr )->GetRangeRef( from, to );
							}
						}
						default:
							throw not_an_array( ref->ToString(), expression->a->m_si );
					}
				}
				default:
					throw not_an_array( expression->a->ToString(), expression->a->m_si );
			}
		}
		case Operator::OT_APPEND: {
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto arrv = ctx->GetVariable( ( (program::Variable*)expression->a )->name, &expression->a->m_si );
					const auto* arr = arrv.Get();
					if ( arr->type != Type::T_ARRAY ) {
						throw operation_not_supported( arr->ToString(), expression->b->ToString() );
					}
					const auto value = EvaluateOperand( ctx, expression->b );
					( (type::Array*)arr )->Append( value );
					return value;
				}
				default:
					throw operation_not_supported( expression->a->ToString(), expression->b->ToString() );
			}
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
			THROW( "operator " + expression->op->Dump() + " not implemented" );
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
			return ctx->GetVariable( ( (program::Variable*)operand )->name, &operand->m_si );
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
			return EvaluateScope( ctx, (program::Scope*)operand );
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
			const auto callable = Deref( ctx, call->m_si, EvaluateExpression( ctx, call->callable ) );
			switch ( callable.Get()->type ) {
				case Type::T_CALLABLE: {
					Callable::function_arguments_t arguments = {};
					for ( const auto& it : call->arguments ) {
						arguments.push_back( Deref( ctx, it->m_si, EvaluateExpression( ctx, it ) ) );
					}
					return ( (Callable*)callable.Get() )->Run( ctx, call->m_si, arguments );
				}
				default:
					throw gse::Exception( EC.INVALID_CALL, "Callable expected, found: " + callable.ToString(), ctx, call->m_si );
			}
		}
		default: {
			THROW( "operand " + operand->ToString() + " not implemented" );
		}
	}
}

const std::string Interpreter::EvaluateString( Context* ctx, const program::Operand* operand ) const {
	const auto result = Deref( ctx, operand->m_si, EvaluateOperand( ctx, operand ) );
	if ( result.Get()->type != Type::T_STRING ) {
		throw gse::Exception( EC.TYPE_ERROR, "Expected string, found: " + operand->ToString(), ctx, operand->m_si );
	}
	return ( (String*)result.Get() )->value;
}

const gse::Value Interpreter::EvaluateRange( Context* ctx, const program::Operand* operand, const bool only_index ) const {
	ASSERT( operand, "index operand missing" );
	const auto& get_index = [ &ctx, &operand ]( const gse::Value& value ) -> size_t {
		const auto* val = value.Get();
		if ( val->type != Type::T_INT ) {
			throw gse::Exception( EC.INVALID_DEREFERENCE, "Invalid range - index must be int: " + val->ToString(), ctx, operand->m_si );
		}
		if ( ( (type::Int*)val )->value < 0 ) {
			throw gse::Exception( EC.INVALID_DEREFERENCE, "Invalid range - index must be positive: " + val->ToString(), ctx, operand->m_si );
		}
		return ( (type::Int*)val )->value;
	};
	switch ( operand->type ) {
		case Operand::OT_VALUE: {
			return VALUE( type::Int, get_index( ( (program::Value*)operand )->value ) );
		}
		case Operand::OT_VARIABLE: {
			return VALUE( type::Int, get_index( ctx->GetVariable( ( (Variable*)operand )->name, &operand->m_si ) ) );
		}
		case Operand::OT_EXPRESSION: {
			const auto result = Deref( ctx, operand->m_si, EvaluateExpression( ctx, (Expression*)operand ) );
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
					THROW( "unexpected index expression result type: " + result.ToString() );
			}
		}
		default: {
			THROW( "unexpected index type: " + operand->ToString() );
		}
	}
}

const bool Interpreter::EvaluateBool( Context* ctx, const program::Operand* operand ) const {
	const auto result = Deref( ctx, operand->m_si, EvaluateOperand( ctx, operand ) );
	if ( result.Get()->type != Type::T_BOOL ) {
		throw gse::Exception( EC.TYPE_ERROR, "Expected bool, found: " + operand->ToString(), ctx, operand->m_si );
	}
	return ( (Bool*)result.Get() )->value;
}

const program::Variable* Interpreter::EvaluateVariable( Context* ctx, const program::Operand* operand ) const {
	if ( operand->type != Operand::OT_VARIABLE ) {
		throw gse::Exception( EC.REFERENCE_ERROR, "Expected variable, found: " + operand->ToString(), ctx, operand->m_si );
	}
	return (Variable*)operand;
}

const std::string Interpreter::EvaluateVarName( Context* ctx, const program::Operand* operand ) const {
	const auto* var = EvaluateVariable( ctx, operand );
	ASSERT( var->hints == Variable::VH_NONE, "unexpected variable hints" );
	return var->name;
}

const gse::Value Interpreter::Deref( Context* ctx, const si_t& si, const gse::Value& value ) const {
	switch ( value.Get()->type ) {
		case Type::T_ARRAYREF: {
			const auto* ref = (ArrayRef*)value.Get();
			return ref->array->Get( ref->index );
		}
		case Type::T_ARRAYRANGEREF: {
			const auto* ref = (ArrayRangeRef*)value.Get();
			ValidateRange( ctx, si, ref->array, ref->from, ref->to );
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

void Interpreter::WriteByRef( Context* ctx, const si_t& si, const gse::Value& ref, const gse::Value& value ) const {
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
			ValidateRange( ctx, si, r->array, r->from, r->to );
			r->array->SetSubArray( r->from, r->to, value );
			break;
		}
		default:
			THROW( "reference expected, found " + ref.ToString() );
	}
}

void Interpreter::ValidateRange( Context* ctx, const si_t& si, const type::Array* array, const std::optional< size_t > from, const std::optional< size_t > to ) const {
	const auto& max = array->value.size() - 1;
	if ( from.has_value() ) {
		if ( from.value() > max ) {
			throw gse::Exception( EC.INVALID_DEREFERENCE, "Invalid range - opening index is behind last element ( " + std::to_string( from.value() ) + " > " + std::to_string( max ) + " )", ctx, si );
		}
	}
	if ( to.has_value() ) {
		if ( to.value() > max ) {
			throw gse::Exception( EC.INVALID_DEREFERENCE, "Invalid range - closing index is behind last element ( " + std::to_string( to.value() ) + " > " + std::to_string( max ) + " )", ctx, si );
		}
		if ( from.has_value() ) {
			if ( from.value() > to.value() ) {
				throw gse::Exception( EC.INVALID_DEREFERENCE, "Invalid range - opening index is behind closing index ( " + std::to_string( from.value() ) + " > " + std::to_string( to.value() ) + " )", ctx, si );
			}
		}
	}
}

Interpreter::Function::Function(
	const Interpreter* runner,
	Context* context,
	const std::vector< std::string >& parameters,
	const program::Program* const program
)
	: runner( runner )
	, context( context )
	, parameters( parameters )
	, program( program ) {
	context->IncRefs();
}

Interpreter::Function::~Function() {
	context->DecRefs();
}

gse::Value Interpreter::Function::Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) {
	ctx->IncRefs();
	auto* subctx = context->ForkContext( ctx, call_si, true, parameters, arguments );
	subctx->IncRefs();
	const auto result = runner->Execute( subctx, program );
	subctx->DecRefs();
	ctx->DecRefs();
	return result;
}

}
}
