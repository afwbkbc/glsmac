#include "Interpreter.h"

#include "gse/context/Context.h"
#include "gse/context/ChildContext.h"
#include "gse/program/Program.h"
#include "gse/program/Object.h"
#include "gse/program/Value.h"
#include "gse/program/Expression.h"
#include "gse/program/SimpleCondition.h"
#include "gse/program/ForCondition.h"
#include "gse/program/ForConditionInOf.h"
#include "gse/program/ForConditionExpressions.h"
#include "gse/program/Operator.h"
#include "gse/program/Operand.h"
#include "gse/program/Variable.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"
#include "gse/program/Array.h"
#include "gse/program/If.h"
#include "gse/program/Statement.h"
#include "gse/program/ElseIf.h"
#include "gse/program/Else.h"
#include "gse/program/While.h"
#include "gse/program/For.h"
#include "gse/program/Try.h"
#include "gse/program/Catch.h"
#include "gse/program/LoopControl.h"
#include "gse/value/Undefined.h"
#include "gse/value/Bool.h"
#include "gse/value/Int.h"
#include "gse/value/Float.h"
#include "gse/value/String.h"
#include "gse/value/Array.h"
#include "gse/value/Object.h"
#include "gse/value/ArrayRef.h"
#include "gse/value/ArrayRangeRef.h"
#include "gse/value/ObjectRef.h"
#include "gse/value/ValueRef.h"
#include "gse/value/Callable.h"
#include "gse/value/Range.h"
#include "gse/value/LoopControl.h"
#include "gse/value/Exception.h"
#include "gse/ExecutionPointer.h"

#if defined( DEBUG ) || defined ( FASTDEBUG )
#include "engine/Engine.h"
#include "config/Config.h"
#include "gc/Space.h"
#endif

namespace gse {

using namespace program;
using namespace value;

namespace runner {

Interpreter::Interpreter( gc::Space* const gc_space )
	: Runner( gc_space ) {}

gse::Value* const Interpreter::Execute( context::Context* ctx, ExecutionPointer& ep, const Program* program ) const {
	return EvaluateScope( ctx, ep, program->body );
}

#if defined( DEBUG ) || defined( FASTDEBUG )
const std::string Interpreter::ToString() {
	return "gse::runner::Interpreter()";
}
#endif

gse::Value* const Interpreter::EvaluateScope( context::Context* ctx, ExecutionPointer& ep, const Scope* scope, bool* returnflag ) const {
	gse::Value* result = nullptr;

	ctx->ForkAndExecute(
		m_gc_space, ctx, scope->m_si, ep, false,
		[ this, &scope, &ep, &result, &returnflag ]( gse::context::ChildContext* const subctx ) {

			for ( const auto& it : scope->body ) {
				bool return_flag = false;
				switch ( it->control_type ) {
					case Control::CT_STATEMENT: {
						result = EvaluateStatement( subctx, ep, (Statement*)it, &return_flag );
						break;
					}
					case Control::CT_CONDITIONAL: {
						result = EvaluateConditional( subctx, ep, (Conditional*)it, false, &return_flag );
						break;
					}
					default:
						THROW( "unexpected control type: " + it->Dump() );
				}
				if ( return_flag ) {
					// got return statement
					if ( returnflag ) {
						*returnflag = true;
					}
					break;
				}
			}

#if defined( DEBUG ) || defined( FASTDEBUG )
			if ( m_are_scope_context_joins_enabled ) {
				subctx->JoinContext();
			}
#endif
		}
	);

	return result;
}

gse::Value* const Interpreter::EvaluateStatement( context::Context* ctx, ExecutionPointer& ep, const Statement* statement, bool* returnflag ) const {
	ASSERT_NOLOG( returnflag, "statement returnflag ptr not set" );
	ASSERT_NOLOG( !*returnflag, "statement but returnflag is already true" );
	const auto result = EvaluateExpression( ctx, ep, statement->body, returnflag );
	if ( *returnflag ) {
		auto* gc_space = m_gc_space;
		return result;
	}
	return nullptr;
}

gse::Value* const Interpreter::EvaluateConditional( context::Context* ctx, ExecutionPointer& ep, const Conditional* conditional, bool is_nested, bool* returnflag ) const {
	auto* gc_space = m_gc_space;
	switch ( conditional->conditional_type ) {
		case Conditional::CT_IF: {
			const auto* c = (If*)conditional;
			if ( EvaluateBool( ctx, ep, c->condition->expression ) ) {
				return EvaluateScope( ctx, ep, c->body, returnflag );
			}
			else if ( c->els ) {
				return EvaluateConditional( ctx, ep, c->els, true, returnflag );
			}
			else {
				return nullptr;
			}
		}
		case Conditional::CT_ELSEIF: {
			if ( !is_nested ) {
				// TODO: move check to parser
				throw gse::Exception( EC.PARSE_ERROR, "Unexpected elseif without if", ctx, conditional->m_si, ep );
			}
			const auto* c = (ElseIf*)conditional;
			if ( EvaluateBool( ctx, ep, c->condition->expression ) ) {
				return EvaluateScope( ctx, ep, c->body, returnflag );
			}
			else if ( c->els ) {
				return EvaluateConditional( ctx, ep, c->els, true, returnflag );
			}
			else {
				return nullptr;
			}
		}
		case Conditional::CT_ELSE: {
			if ( !is_nested ) {
				// TODO: move check to parser
				throw gse::Exception( EC.PARSE_ERROR, "Unexpected else without if", ctx, conditional->m_si, ep );
			}
			const auto* c = (Else*)conditional;
			return EvaluateScope( ctx, ep, c->body, returnflag );
		}
		case Conditional::CT_WHILE: {
			const auto* c = (While*)conditional;
			gse::Value* result = nullptr;
			bool need_break = false;
			bool need_clear = false;
			while ( EvaluateBool( ctx, ep, c->condition->expression ) ) {
				result = EvaluateScope( ctx, ep, c->body, returnflag );
				CheckBreakCondition( result, &need_break, &need_clear );
				if ( need_clear ) {
					result = nullptr;
				}
				if ( need_break || ( returnflag && *returnflag ) ) {
					break;
				}
			}
			return result;
		}
		case Conditional::CT_FOR: {
			const auto* c = (For*)conditional;
			gse::Value* result = nullptr;
			bool need_break = false;
			bool need_clear = false;
			switch ( c->condition->for_type ) {
				case ForCondition::FCT_EXPRESSIONS: {
					const auto* condition = (ForConditionExpressions*)c->condition;
					bool return_flag = false;
					EvaluateExpression( ctx, ep, condition->init, returnflag );
					while ( EvaluateBool( ctx, ep, condition->check ) ) {
						result = EvaluateScope( ctx, ep, c->body );
						CheckBreakCondition( result, &need_break, &need_clear );
						if ( need_break || ( returnflag && *returnflag ) ) {
							if ( need_clear ) {
								result = nullptr;
							}
							break;
						}
						EvaluateExpression( ctx, ep, condition->iterate, returnflag );
					}
					break;
				}
				case ForCondition::FCT_IN_OF: {
					const auto* condition = (ForConditionInOf*)c->condition;
					const auto target = Deref( ctx, condition->m_si, ep, EvaluateExpression( ctx, ep, condition->expression ) );
					ctx->ForkAndExecute(
						m_gc_space, ctx, condition->m_si, ep, false, [ this, &gc_space, &target, &condition, &result, &ep, &c, &need_break, &need_clear, &returnflag ]( gse::context::ChildContext* const subctx ) {

							switch ( target->type ) {
								case gse::Value::T_ARRAY: {
									const auto* arr = (value::Array*)target;
									switch ( condition->for_inof_type ) {
										case ForConditionInOf::FIC_IN: {
											for ( size_t i = 0 ; i < arr->value.size() ; i++ ) {
												subctx->CreateConst( condition->variable->name, VALUE( Int, , i ), condition->m_si, ep );
												result = EvaluateScope( subctx, ep, c->body, returnflag );
												subctx->DestroyVariable( condition->variable->name, condition->m_si, ep );
												CheckBreakCondition( result, &need_break, &need_clear );
												if ( need_break || ( returnflag && *returnflag ) ) {
													if ( need_clear ) {
														result = nullptr;
													}
													break;
												}
											}
											break;
										}
										case ForConditionInOf::FIC_OF: {
											for ( const auto& v : arr->value ) {
												subctx->CreateConst( condition->variable->name, v, condition->m_si, ep );
												result = EvaluateScope( subctx, ep, c->body, returnflag );
												subctx->DestroyVariable( condition->variable->name, condition->m_si, ep );
												CheckBreakCondition( result, &need_break, &need_clear );
												if ( need_break || ( returnflag && *returnflag ) ) {
													if ( need_clear ) {
														result = nullptr;
													}
													break;
												}
											}
											break;
										}
										default:
											THROW( "unexpected for in_of condition type: " + std::to_string( condition->for_inof_type ) );
									}
									break;
								}
								case gse::Value::T_OBJECT: {
									const auto* obj = (value::Object*)target;
									if ( condition->for_inof_type != ForConditionInOf::FIC_IN && condition->for_inof_type != ForConditionInOf::FIC_OF ) {
										THROW( "unexpected for in_of condition type: " + std::to_string( condition->for_inof_type ) );
									}
									for ( const auto& v : obj->value ) {
										subctx->CreateConst(
											condition->variable->name, condition->for_inof_type == ForConditionInOf::FIC_IN
												? VALUE( String, , v.first )
												: v.second, condition->m_si, ep
										);
										result = EvaluateScope( subctx, ep, c->body, returnflag );
										subctx->DestroyVariable( condition->variable->name, condition->m_si, ep );
										if ( returnflag && *returnflag ) {
											break;
										}
									}
									break;
								}
								default:
									THROW( "unexpected type for iteration (" + target->GetTypeString() + "): " + target->ToString() );
							}
						}
					);
					break;
				}
				default:
					THROW( "unexpected for condition type: " + std::to_string( c->condition->for_type ) );
			}
			if ( result && result->type == gse::Value::T_LOOPCONTROL ) {
				// we don't want to break out from parent scope
				if ( returnflag ) {
					*returnflag = false;
				}
				return nullptr;
			}
			return result;
		}
		case Conditional::CT_TRY: {
			const auto* c = (Try*)conditional;
			try {
				return EvaluateScope( ctx, ep, c->body, returnflag );
			}
			catch ( const gse::Exception& e ) {
				const auto* h = c->handlers->handlers;
				auto it = h->properties.find( e.class_name );
				if ( it == h->properties.end() ) {
					it = h->properties.find( "" ); // check for default handler too
				}
				if ( it != h->properties.end() ) {
					const auto f = EvaluateExpression( ctx, ep, it->second, returnflag );
					if ( f->type != gse::Value::T_CALLABLE ) {
						// TODO: move check to parser
						throw gse::Exception( EC.PARSE_ERROR, "Expected catch block, found: " + f->ToString(), ctx, it->second->m_si, ep );
					}
					auto si = e.si;
					auto* func = (Function*)f;
					const auto result = func->Run(
						m_gc_space,
						ctx,
						it->second->m_si,
						ep,
						{
							VALUEEXT( value::Exception, GSE_CALL, e, e.GetStackTrace() )
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

gse::Value* const Interpreter::EvaluateExpression( context::Context* ctx, ExecutionPointer& ep, const Expression* expression, bool* returnflag ) const {
	auto* gc_space = m_gc_space;
	if ( !expression->op ) {
		ASSERT( !expression->b, "expression has second operand but no operator" );
		ASSERT( expression->a, "expression is empty" );
		return EvaluateOperand( ctx, ep, expression->a );
	}
	const auto& operation_not_supported = [ expression, &ctx, &ep ]( const std::string& a, const std::string& b ) -> gse::Exception {
		return gse::Exception( EC.OPERATION_NOT_SUPPORTED, "Operation " + expression->op->ToString() + " is not supported between " + a + " and " + b, ctx, expression->op->m_si, ep );
	};
	const auto& operation_not_supported_not_array = [ expression, &ctx, &ep ]( const std::string& a ) -> gse::Exception {
		return gse::Exception( EC.OPERATION_NOT_SUPPORTED, "Operation " + expression->op->ToString() + " is not supported: " + a + " is not array", ctx, expression->op->m_si, ep );
	};
	const auto& math_error = [ expression, &ctx, &ep ]( const std::string& reason ) -> gse::Exception {
		return gse::Exception( EC.MATH_ERROR, reason, ctx, expression->op->m_si, ep );
	};
	switch ( expression->op->op ) {
		case OT_RETURN: {
			ASSERT( returnflag, "return keyword not allowed here" );
			ASSERT( !*returnflag, "already returning" );
			ASSERT( !expression->a, "unexpected left operand before return" );
			ASSERT( expression->b, "return value or expression expected" );
			*returnflag = true;
			return Deref( ctx, expression->b->m_si, ep, EvaluateOperand( ctx, ep, expression->b ) );
		}
		case OT_BREAK: {
			ASSERT( returnflag, "break keyword not allowed here" );
			ASSERT( !*returnflag, "already returning" );
			*returnflag = true;
			return EvaluateOperand( ctx, ep, expression->b );
		}
		case OT_CONTINUE: {
			ASSERT( returnflag, "continue keyword not allowed here" );
			ASSERT( !*returnflag, "already returning" );
			*returnflag = true;
			return EvaluateOperand( ctx, ep, expression->b );
		}
		case OT_THROW: {
			ASSERT( !expression->a, "unexpected left operand before throw" );
			const auto& invalid_error_definition = [ expression, &ctx, &ep ]() -> gse::Exception {
				return gse::Exception(
					EC.INVALID_CALL, "Invalid error definition. Expected: ErrorType(reason), found: " + ( expression->b
						? expression->b->ToString()
						: "Nothing"
					), ctx, expression->a->m_si, ep
				);
			};
			if ( !expression->b || expression->b->type != Operand::OT_CALL ) {
				throw invalid_error_definition();
			}
			const auto* e = (Call*)expression->b; // it's not a call but it looks like a call
			if (
				!e->callable->a ||
					e->callable->a->type != Operand::OT_VARIABLE ||
					e->callable->op ||
					e->callable->b ||
					e->arguments.size() != 1 ) {
				throw invalid_error_definition();
			}
			const auto reason = EvaluateExpression( ctx, ep, e->arguments[ 0 ] );
			if ( reason->type != gse::Value::T_STRING ) {
				throw invalid_error_definition();
			}

			throw gse::Exception(
				( (Variable*)e->callable->a )->name,
				( (String*)reason )->value,
				ctx,
				expression->op->m_si,
				ep
			);
		}
		case OT_ASSIGN: {
			ASSERT( expression->a, "missing assignment target" );
			auto result = Deref( ctx, expression->b->m_si, ep, EvaluateOperand( ctx, ep, expression->b ) );
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto* var = (Variable*)expression->a;
					if ( var->name[ 0 ] == '#' ) {
						throw gse::Exception( EC.INVALID_ASSIGNMENT, "Can't assign to builtin: " + var->name, ctx, var->m_si, ep );
					}
					if ( var->hints & VH_CREATE_VAR ) {
						ctx->CreateVariable( var->name, result, expression->a->m_si, ep );
					}
					else if ( var->hints & VH_CREATE_CONST ) {
						ctx->CreateConst( var->name, result, expression->a->m_si, ep );
					}
					else {
						ctx->UpdateVariable( var->name, result, expression->a->m_si, ep );
					}
					break;
				}
				case Operand::OT_EXPRESSION: {
					// property of object?
					const auto target = EvaluateExpression( ctx, ep, (Expression*)expression->a );
					// assign to reference
					WriteByRef( ctx, expression->a->m_si, ep, target, result );
					break;
				}
				default:
					throw gse::Exception( EC.INVALID_ASSIGNMENT, (std::string)"Can't assign " + result->ToString() + " to " + expression->a->ToString(), ctx, expression->a->m_si, ep );
			}
			return result;
		}
		case OT_NOT: {
			ASSERT( !expression->a, "unary not may not have left operand" );
			return VALUE( Bool, , !EvaluateBool( ctx, ep, expression->b ) );
		}
#define CMP_OP( _op ) { \
        return VALUE( Bool,, \
            *Deref( ctx, expression->a->m_si, ep, EvaluateOperand( ctx, ep, expression->a ) ) \
                _op \
            *Deref( ctx, expression->b->m_si, ep, EvaluateOperand( ctx, ep, expression->b ) ) \
            ); \
}
		case OT_EQ: CMP_OP( == )
		case OT_NE: CMP_OP( != )
		case OT_LT: CMP_OP( < )
		case OT_LTE: CMP_OP( <= )
		case OT_GT: CMP_OP( > )
		case OT_GTE: CMP_OP( >= )
#undef CMP_OP
#define CMP_BOOL( _op ) { \
        return VALUE( Bool,, \
            EvaluateBool( ctx, ep, expression->a ) _op \
                EvaluateBool( ctx, ep, expression->b ) \
            ); \
}
		case OT_AND: CMP_BOOL( && )
		case OT_OR: CMP_BOOL( || )
#undef CMP_BOOL
#define MATH_OP_BEGIN( _op, _allow_b_zero ) \
            const auto av = Deref( ctx, expression->a->m_si, ep, EvaluateOperand( ctx, ep, expression->a ) ); \
            const auto bv = Deref( ctx, expression->b->m_si, ep, EvaluateOperand( ctx, ep, expression->b ) ); \
            const auto* a = av; \
            const auto* b = bv; \
            if ( a->type != b->type ) { \
                throw operation_not_supported( a->ToString(), b->ToString() ); \
            } \
            switch ( a->type ) { \
                case gse::Value::T_INT: { \
                    const auto bval = ( (Int*)b )->value; \
                    if ( !_allow_b_zero && bval == 0 ) { \
                        throw math_error( "Division by zero" ); \
                    } \
                    return VALUE( Int,, ( (Int*)a )->value _op bval ); \
                }
#define MATH_OP_BEGIN_F( _op, _allow_b_zero ) \
        MATH_OP_BEGIN( _op, _allow_b_zero ) \
            case gse::Value::T_FLOAT: { \
                const auto bval = ( (Float*)b )->value; \
                if ( !_allow_b_zero && bval == 0.0f ) { \
                    throw math_error( "Division by zero" ); \
                } \
                return VALUE( Float,, ( (Float*)a )->value _op bval ); \
            }
#define MATH_OP_END() \
                default: \
                    throw operation_not_supported( a->ToString(), b->ToString() ); \
            }
#define MATH_OP( _op, _allow_b_zero ) \
        MATH_OP_BEGIN_F( _op, _allow_b_zero ) \
        MATH_OP_END()
		case OT_ADD: {
			MATH_OP_BEGIN_F( +, true )
				case gse::Value::T_STRING:
					return VALUE( String, , ( (String*)a )->value + ( (String*)b )->value );
				case gse::Value::T_ARRAY: {
					array_elements_t elements = ( (value::Array*)a )->value;
					elements.insert( elements.end(), ( (value::Array*)b )->value.begin(), ( (value::Array*)b )->value.end() );
					return VALUE( value::Array, , elements );
				}
				case gse::Value::T_OBJECT: {
					object_properties_t properties = ( (value::Object*)a )->value;
					for ( const auto& it : ( (value::Object*)b )->value ) {
						if ( properties.find( it.first ) != properties.end() ) {
							throw gse::Exception( EC.OPERATION_FAILED, "Can't concatenate objects - duplicate key found: " + it.first, ctx, expression->op->m_si, ep );
						}
						properties.insert_or_assign( it.first, it.second );
					}
					auto si = expression->m_si;
					return VALUEEXT( value::Object, GSE_CALL, properties );
				}
			MATH_OP_END()
		}
		case OT_SUB: {
			MATH_OP( -, true )
		}
		case OT_MULT: {
			MATH_OP( *, true )
		}
		case OT_DIV: {
			MATH_OP( /, false )
		}
		case OT_MOD: {
			MATH_OP_BEGIN( %, false )
			MATH_OP_END()
		}
#undef MATH_OP
#undef MATH_OP_BEGIN
#undef MATH_OP_BEGIN_F
#undef MATH_OP_END
#define MATH_OP( _op ) \
        if ( expression->a ) { \
            ASSERT( !expression->b, "only one operand required, found two" ); \
            const auto varname = EvaluateVarName( ctx, ep, expression->a ); \
            gse::Value* value = ctx->GetVariable( varname, expression->a->m_si, ep ); \
            if ( value->type != gse::Value::T_INT ) { \
                throw gse::Exception( EC.TYPE_ERROR, "Expected int, found: " + expression->a->ToString(), ctx, expression->a->m_si, ep ); \
            } \
            ctx->UpdateVariable( varname, VALUE( Int,, ( (Int*)value )->value _op 1 ), expression->a->m_si, ep ); \
            return value; \
        } \
        else if ( expression->b ) { \
            const auto varname = EvaluateVarName( ctx, ep, expression->b ); \
            const gse::Value* value = ctx->GetVariable( varname, expression->b->m_si, ep ); \
            if ( value->type != gse::Value::T_INT ) { \
                throw gse::Exception( EC.TYPE_ERROR, "Expected int, found: " + expression->b->ToString(), ctx, expression->b->m_si, ep ); \
            } \
            const auto result = VALUE( Int,, ( (Int*)value )->value _op 1 ); \
            ctx->UpdateVariable( varname, result, expression->b->m_si, ep ); \
            return result; \
        } \
        else { \
            THROW( "operands not found" ); \
        }
		case OT_INC: {
			MATH_OP( + )
		}
		case OT_DEC: {
			MATH_OP( - )
		}
#undef MATH_OP
#define MATH_OP_BEGIN( _op ) \
        const auto varname = EvaluateVarName( ctx, ep, expression->a ); \
        const auto av = Deref( ctx, expression->a->m_si, ep, ctx->GetVariable( varname, expression->a->m_si, ep ) ); \
        const auto bv = Deref( ctx, expression->b->m_si, ep, EvaluateOperand( ctx, ep, expression->b ) ); \
        const auto* a = av; \
        const auto* b = bv; \
        if ( a->type != b->type ) {                                 \
            throw operation_not_supported( a->ToString(), b->ToString() ); \
        } \
        gse::Value* result = nullptr; \
        switch ( a->type ) { \
            case gse::Value::T_INT: { \
                result = VALUE( Int,, ( (Int*)a )->value _op ( (Int*)b )->value ); \
                break; \
            }
#define MATH_OP_BEGIN_F( _op ) \
        MATH_OP_BEGIN( _op ) \
            case gse::Value::T_FLOAT: { \
                result = VALUE( Float,, ( (Float*)a )->value _op ( (Float*)b )->value ); \
                break; \
            }
#define MATH_OP_END() \
            default:  \
                throw operation_not_supported( a->ToString(), b->ToString() ); \
        } \
        ctx->UpdateVariable( varname, result, expression->a->m_si, ep ); \
        return result;
#define MATH_OP( _op ) \
        MATH_OP_BEGIN_F( _op ) \
        MATH_OP_END()
		case OT_INC_BY: {
			MATH_OP_BEGIN_F( + )
				case gse::Value::T_STRING: {
					result = VALUE( String, , ( (String*)a )->value + ( (String*)b )->value );
					break;
				}
				case gse::Value::T_ARRAY: {
					array_elements_t elements = ( (value::Array*)a )->value;
					elements.insert( elements.end(), ( (value::Array*)b )->value.begin(), ( (value::Array*)b )->value.end() );
					result = VALUE( value::Array, , elements );
					break;
				}
				case gse::Value::T_OBJECT: {
					object_properties_t properties = ( (value::Object*)a )->value;
					for ( const auto& it : ( (value::Object*)b )->value ) {
						if ( properties.find( it.first ) != properties.end() ) {
							throw gse::Exception( EC.OPERATION_FAILED, "Can't append object - duplicate key found: " + it.first, ctx, expression->op->m_si, ep );
						}
						properties.insert_or_assign( it.first, it.second );
					}
					auto si = expression->m_si;
					return VALUEEXT( value::Object, GSE_CALL, properties );
				}
			MATH_OP_END()
		}
		case OT_DEC_BY: {
			MATH_OP( - )
		}
		case OT_MULT_BY: {
			MATH_OP( * )
		}
		case OT_DIV_BY: {
			MATH_OP( / )
		}
		case OT_MOD_BY: {
			MATH_OP_BEGIN( % )
			MATH_OP_END()
		}
#undef MATH_OP
#undef MATH_OP_BEGIN
#undef MATH_OP_BEGIN_F
#undef MATH_OP_END
		case OT_CHILD: {
			ASSERT( expression->a, "parent object expected" );
			const auto childname = EvaluateVarName( ctx, ep, expression->b );
			const auto& not_an_object = [ &ctx, expression, &ep, &childname ]( const std::string& what, const si_t& si ) -> gse::Exception {
				return gse::Exception( EC.INVALID_DEREFERENCE, "Could not get ." + childname + " of non-object: " + what, ctx, expression->op->m_si, ep );
			};
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto objv = ctx->GetVariable( ( (Variable*)expression->a )->name, expression->a->m_si, ep );
					const auto* obj = objv;
					if ( obj->type == gse::Value::T_VALUEREF ) {
						obj = ( (ValueRef*)obj )->target;
					}
					if ( obj->type != gse::Value::T_OBJECT ) {
						throw not_an_object( obj->ToString(), expression->a->m_si );
					}
					return ( (value::Object*)obj )->GetRef( childname );
				}
				case Operand::OT_OBJECT:
				case Operand::OT_CALL: {
					const auto objv = EvaluateOperand( ctx, ep, expression->a );
					const auto* obj = objv;
					ASSERT( obj->type == gse::Value::T_OBJECT, "parent is not object: " + obj->Dump() );
					return ( (value::Object*)obj )->Get( childname );
				}
				case Operand::OT_EXPRESSION: {
					const auto objv = Deref( ctx, expression->a->m_si, ep, EvaluateExpression( ctx, ep, (Expression*)expression->a ) );
					const auto* obj = objv;
					if ( obj->type != gse::Value::T_OBJECT ) {
						throw not_an_object( obj->ToString(), expression->a->m_si );
					}
					return ( (value::Object*)obj )->GetRef( childname );
				}
				default: {
					throw not_an_object( expression->a->ToString(), expression->a->m_si );
				}
			}
		}
		case OT_AT: {
			ASSERT( expression->a, "parent array expected" );
			std::optional< size_t > index, from, to;
			const auto valv = EvaluateRange( ctx, ep, expression->b );
			const auto* val = valv;
			switch ( val->type ) {
				case gse::Value::T_INT: {
					index = ( (Int*)val )->value;
					from = std::nullopt;
					to = std::nullopt;
					break;
				}
				case gse::Value::T_RANGE: {
					const auto* range = (Range*)val;
					index = std::nullopt;
					from = range->from;
					to = range->to;
					break;
				}
				default:
					THROW( "unexpected index type: " + val->ToString() );
			}
			const auto& not_an_array = [ &ctx, &ep, &index, &from, &to ]( const std::string& what, const si_t& si ) -> gse::Exception {
				return gse::Exception(
					EC.INVALID_DEREFERENCE, "Could not get " +
						( index.has_value()
							? "index " + std::to_string( index.value() )
							: "range [ " + std::to_string( from.value() ) + " : " + std::to_string( to.value() )
						) +
						" of non-array: " + what, ctx, si, ep
				);
			};
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto arrv = ctx->GetVariable( ( (Variable*)expression->a )->name, expression->a->m_si, ep );
					const auto* arr = arrv;
					if ( arr->type != gse::Value::T_ARRAY ) {
						throw not_an_array( arr->ToString(), expression->a->m_si );
					}
					if ( index.has_value() ) {
						return ( (value::Array*)arr )->GetRef( index.value() );
					}
					else {
						return ( (value::Array*)arr )->GetRangeRef( from, to );
					}
				}
				case Operand::OT_ARRAY: {
					const auto arrv = EvaluateOperand( ctx, ep, expression->a );
					const auto* arr = arrv;
					ASSERT( arr->type == gse::Value::T_ARRAY, "parent is not array: " + arr->Dump() );
					if ( index.has_value() ) {
						return ( (value::Array*)arr )->Get( index.value() );
					}
					else {
						ValidateRange( ctx, expression->b->m_si, ep, (value::Array*)arr, from, to );
						return ( (value::Array*)arr )->GetSubArray( from, to );
					}
				}
				case Operand::OT_EXPRESSION: {
					const auto refv = EvaluateExpression( ctx, ep, (Expression*)expression->a );
					const auto* ref = refv;
					switch ( ref->type ) {
						case gse::Value::T_STRING: {
							const auto& v = ( (value::String*)ref )->value;
							const auto f = from.has_value()
								? from.value()
								: 0;
							if ( to.has_value() ) {
								return VALUE( String, , v.substr( f, to.value() - f ) );
							}
							else {
								return VALUE( String, , v.substr( f ) );
							}
						}
						case gse::Value::T_ARRAY: {
							if ( index.has_value() ) {
								return ( (value::Array*)ref )->Get( index.value() );
							}
							else {
								return ( (value::Array*)ref )->GetRangeRef( from, to );
							}
						}
						case gse::Value::T_ARRAYREF: {
							const auto* r = (ArrayRef*)ref;
							const auto arrv = r->array->Get( r->index );
							const auto* arr = arrv;
							if ( arr->type != gse::Value::T_ARRAY ) {
								throw not_an_array( arr->ToString(), expression->a->m_si );
							}
							if ( index.has_value() ) {
								return ( (value::Array*)arr )->GetRef( index.value() );
							}
							else {
								return ( (value::Array*)arr )->GetRangeRef( from, to );
							}
						}
						case gse::Value::T_ARRAYRANGEREF: {
							THROW( "TODO: T_ARRAYRANGEREF" );
						}
						case gse::Value::T_LOOPCONTROL: {
							THROW( "TODO: T_LOOPCONTROL" );
						}
						case gse::Value::T_OBJECTREF: {
							const auto arrv = Deref( ctx, expression->a->m_si, ep, refv );
							const auto* arr = arrv;
							if ( arr->type != gse::Value::T_ARRAY ) {
								throw not_an_array( arr->ToString(), expression->a->m_si );
							}
							if ( index.has_value() ) {
								return ( (value::Array*)arr )->GetRef( index.value() );
							}
							else {
								return ( (value::Array*)arr )->GetRangeRef( from, to );
							}
						}
						case gse::Value::T_VALUEREF: {
							THROW( "TODO: T_VALUEREF" );
						}
						default:
							throw not_an_array( ref->ToString(), expression->a->m_si );
					}
				}
				default:
					throw not_an_array( expression->a->ToString(), expression->a->m_si );
			}
		}
		case OT_PUSH: {
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto arrv = ctx->GetVariable( ( (Variable*)expression->a )->name, expression->a->m_si, ep );
					const auto* arr = arrv;
					if ( arr->type != gse::Value::T_ARRAY ) {
						throw operation_not_supported_not_array( expression->a->ToString() );
					}
					gse::Value* value = EvaluateOperand( ctx, ep, expression->b );
					( (value::Array*)arr )->Append( value );
					return value;
				}
				default:
					throw operation_not_supported_not_array( expression->a->ToString() );
			}
		}
		case OT_POP: {
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto arrv = ctx->GetVariable( ( (Variable*)expression->a )->name, expression->a->m_si, ep );
					const auto* arr = arrv;
					if ( arr->type != gse::Value::T_ARRAY ) {
						throw operation_not_supported_not_array( expression->a->ToString() );
					}
					auto& a = ( (value::Array*)arr )->value;
					gse::Value* value = nullptr;
					if ( !a.empty() ) {
						value = a.back();
						a.pop_back();
					}
					return value;
				}
				default:
					throw operation_not_supported_not_array( expression->a->ToString() );
			}
		}
		case OT_ERASE: {
			switch ( expression->a->type ) {
				case Operand::OT_VARIABLE: {
					const auto arrv = ctx->GetVariable( ( (Variable*)expression->a )->name, expression->a->m_si, ep );
					const auto* arr = arrv;
					if ( arr->type != gse::Value::T_ARRAY ) {
						throw operation_not_supported_not_array( expression->a->ToString() );
					}
					auto& a = ( (value::Array*)arr )->value;
					const auto idxval = EvaluateOperand( ctx, ep, expression->b );
					if ( idxval->type != gse::Value::T_INT ) {
						throw operation_not_supported( expression->a->ToString(), idxval->ToString() );
					}
					const auto idx = ( (value::Int*)idxval )->value;
					if ( idx < 0 ) {
						throw operation_not_supported( expression->a->ToString(), idxval->ToString() );
					}
					gse::Value* value = nullptr;
					if ( idx < a.size() ) {
						value = a.at( idx );
						a.erase( a.begin() + idx );
					}
					return value;
				}
				default:
					throw operation_not_supported_not_array( expression->a->ToString() );
			}
		}
		case OT_RANGE: {

			std::optional< size_t > from = std::nullopt;
			std::optional< size_t > to = std::nullopt;

			if ( expression->a ) {
				const auto fromv = EvaluateRange( ctx, ep, expression->a, true );
				const auto* fromr = fromv;
				ASSERT( fromr->type == gse::Value::T_INT, "int expected here" );
				from = ( (Int*)fromr )->value;
			}
			if ( expression->b ) {
				const auto tov = EvaluateRange( ctx, ep, expression->b, true );
				const auto* tor = tov;
				ASSERT( tor->type == gse::Value::T_INT, "int expected here" );
				to = ( (Int*)tor )->value;
			}

			return VALUE( Range, , from, to );
		}
		default: {
			THROW( "operator " + expression->op->Dump() + " not implemented" );
		}
	}
}

gse::Value* const Interpreter::EvaluateOperand( context::Context* ctx, ExecutionPointer& ep, const Operand* operand ) const {
	auto* gc_space = m_gc_space;
	ASSERT( operand, "operand is null" );
	switch ( operand->type ) {
		case Operand::OT_VALUE: {
			return ( (program::Value*)operand )->value;
		}
		case Operand::OT_VARIABLE: {
			return ctx->GetVariable( ( (Variable*)operand )->name, operand->m_si, ep );
		}
		case Operand::OT_ARRAY: {
			auto* arr = (program::Array*)operand;
			array_elements_t elements;
			for ( const auto& it : arr->elements ) {
				elements.push_back( EvaluateExpression( ctx, ep, it ) );
			}
			return VALUE( value::Array, , elements );
		}
		case Operand::OT_OBJECT: {
			gse::Value* result = nullptr;
			const auto* obj = (program::Object*)operand;
			for ( const auto& it : obj->ordered_properties ) {
				if ( it.first == "this" ) {
					throw gse::Exception( EC.INVALID_ASSIGNMENT, "'this' can't be overwritten", ctx, it.second->m_si, ep );
				}
			}
			result = VALUE( value::Object, , ctx, obj->m_si, ep, object_properties_t{} );
			auto& properties = ( (value::Object*)result )->value;
			for ( const auto& it : obj->ordered_properties ) {
				properties.insert_or_assign( it.first, EvaluateExpression( ( (value::Object*)result )->GetContext(), ep, it.second )->Clone() );
			}
			return result;
		}
		case Operand::OT_SCOPE: {
			return EvaluateScope( ctx, ep, (Scope*)operand );
		}
		case Operand::OT_EXPRESSION: {
			return EvaluateExpression( ctx, ep, ( (Expression*)operand ) );
		}
		case Operand::OT_FUNCTION: {
			const auto* func = (program::Function*)operand;
			std::vector< std::string > parameters = {};
			for ( const auto& it : func->parameters ) {
				ASSERT( it->hints == VH_NONE, "function parameters can't have modifiers" );
				parameters.push_back( it->name );
			}
			return VALUE( Function, , this, ctx, parameters, new Program( func->body, false ) );
		}
		case Operand::OT_CALL: {
			const auto* call = (Call*)operand;
			const auto* callable = Deref( ctx, call->m_si, ep, EvaluateExpression( ctx, ep, call->callable ) );
			ASSERT( callable, "callable is null" );
			switch ( callable->type ) {
				case gse::Value::T_CALLABLE: {
					function_arguments_t arguments = {};
					for ( const auto& it : call->arguments ) {
						arguments.push_back( Deref( ctx, it->m_si, ep, EvaluateExpression( ctx, ep, it ) ) );
					}
					return ( (Callable*)callable )->Run( gc_space, ctx, call->m_si, ep, arguments );
				}
				default:
					throw gse::Exception( EC.INVALID_CALL, "Callable expected, found: " + callable->ToString(), ctx, call->m_si, ep );
			}
		}
		case Operand::OT_LOOP_CONTROL: {
			return VALUE( value::LoopControl, , ( (program::LoopControl*)operand )->loop_control_type );
		}
		default: {
			THROW( "operand " + operand->ToString() + " not implemented" );
		}
	}
}

const std::string Interpreter::EvaluateString( context::Context* ctx, ExecutionPointer& ep, const Operand* operand ) const {
	const auto result = Deref( ctx, operand->m_si, ep, EvaluateOperand( ctx, ep, operand ) );
	if ( result->type != gse::Value::T_STRING ) {
		throw gse::Exception( EC.TYPE_ERROR, "Expected string, found: " + operand->ToString(), ctx, operand->m_si, ep );
	}
	return ( (String*)result )->value;
}

gse::Value* const Interpreter::EvaluateRange( context::Context* ctx, ExecutionPointer& ep, const Operand* operand, const bool only_index ) const {
	auto* gc_space = m_gc_space;
	ASSERT( operand, "index operand missing" );
	const auto& get_index = [ &ctx, &ep, &operand ]( gse::Value* const value ) -> size_t {
		const auto* val = value;
		if ( val->type != gse::Value::T_INT ) {
			throw gse::Exception( EC.INVALID_DEREFERENCE, "Invalid range - index must be int: " + val->ToString(), ctx, operand->m_si, ep );
		}
		if ( ( (Int*)val )->value < 0 ) {
			throw gse::Exception( EC.INVALID_DEREFERENCE, "Invalid range - index must be positive: " + val->ToString(), ctx, operand->m_si, ep );
		}
		return ( (Int*)val )->value;
	};
	switch ( operand->type ) {
		case Operand::OT_VALUE: {
			return VALUE( Int, , get_index( ( (program::Value*)operand )->value ) );
		}
		case Operand::OT_VARIABLE: {
			return VALUE( Int, , get_index( ctx->GetVariable( ( (Variable*)operand )->name, operand->m_si, ep ) ) );
		}
		case Operand::OT_EXPRESSION: {
			const auto result = Deref( ctx, operand->m_si, ep, EvaluateExpression( ctx, ep, (Expression*)operand ) );
			switch ( result->type ) {
				case gse::Value::T_INT: {
					return VALUE( Int, , get_index( result ) );
				}
				case gse::Value::T_RANGE: {
					ASSERT( !only_index, "range not allowed here" );
					const auto* range = (Range*)result;
					return VALUE( Range, , range->from, range->to );
				}
				default:
					THROW( "unexpected index expression result type: " + result->ToString() );
			}
		}
		default: {
			THROW( "unexpected index type: " + operand->ToString() );
		}
	}
}

const bool Interpreter::EvaluateBool( context::Context* ctx, ExecutionPointer& ep, const Operand* operand ) const {
	const auto result = Deref( ctx, operand->m_si, ep, EvaluateOperand( ctx, ep, operand ) );
	if ( result->type != gse::Value::T_BOOL ) {
		throw gse::Exception( EC.TYPE_ERROR, "Expected bool, found: " + operand->ToString(), ctx, operand->m_si, ep );
	}
	return ( (Bool*)result )->value;
}

const Variable* Interpreter::EvaluateVariable( context::Context* ctx, ExecutionPointer& ep, const Operand* operand ) const {
	if ( operand->type != Operand::OT_VARIABLE ) {
		throw gse::Exception( EC.REFERENCE_ERROR, "Expected variable, found: " + operand->ToString(), ctx, operand->m_si, ep );
	}
	return (Variable*)operand;
}

const std::string Interpreter::EvaluateVarName( context::Context* ctx, ExecutionPointer& ep, const Operand* operand ) const {
	const auto* var = EvaluateVariable( ctx, ep, operand );
	ASSERT( var->hints == VH_NONE, "unexpected variable hints" );
	return var->name;
}

gse::Value* const Interpreter::Deref( context::Context* ctx, const si_t& si, ExecutionPointer& ep, gse::Value* const value ) const {
	if ( value ) {
		switch ( value->type ) {
			case gse::Value::T_ARRAYREF: {
				const auto* ref = (ArrayRef*)value;
				return ref->array->Get( ref->index );
			}
			case gse::Value::T_ARRAYRANGEREF: {
				const auto* ref = (ArrayRangeRef*)value;
				ValidateRange( ctx, si, ep, ref->array, ref->from, ref->to );
				return ref->array->GetSubArray( ref->from, ref->to );
			}
			case gse::Value::T_OBJECTREF: {
				const auto* ref = (ObjectRef*)value;
				return ref->object->Get( ref->key );
			}
			case gse::Value::T_VALUEREF: {
				THROW( "TODO: T_VALUEREF" );
			}
			default: {
			}
		}
	}
	return value;
}

void Interpreter::WriteByRef( context::Context* ctx, const si_t& si, ExecutionPointer& ep, gse::Value* const ref, gse::Value* const value ) const {
	auto* gc_space = m_gc_space;
	switch ( ref->type ) {
		case gse::Value::T_OBJECTREF: {
			const auto* r = (ObjectRef*)ref;
			r->object->Set( r->key, value, GSE_CALL );
			break;
		}
		case gse::Value::T_ARRAYREF: {
			const auto* r = (ArrayRef*)ref;
			r->array->Set( r->index, value );
			break;
		}
		case gse::Value::T_ARRAYRANGEREF: {
			const auto* r = (ArrayRangeRef*)ref;
			ValidateRange( ctx, si, ep, r->array, r->from, r->to );
			r->array->SetSubArray( r->from, r->to, value );
			break;
		}
		case gse::Value::T_VALUEREF: {
			THROW( "TODO: T_VALUEREF" );
		}
		default:
			THROW( "reference expected, found " + ref->ToString() );
	}
}

void Interpreter::ValidateRange( context::Context* ctx, const si_t& si, ExecutionPointer& ep, const value::Array* array, const std::optional< size_t > from, const std::optional< size_t > to ) const {
	const auto& max = array->value.size() - 1;
	if ( from.has_value() ) {
		if ( from.value() > max ) {
			GSE_ERROR( EC.INVALID_DEREFERENCE, "Invalid range - opening index is behind last element ( " + std::to_string( from.value() ) + " > " + std::to_string( max ) + " )" );
		}
	}
	if ( to.has_value() ) {
		if ( to.value() > max ) {
			GSE_ERROR( EC.INVALID_DEREFERENCE, "Invalid range - closing index is behind last element ( " + std::to_string( to.value() ) + " > " + std::to_string( max ) + " )" );
		}
		if ( from.has_value() ) {
			if ( from.value() > to.value() ) {
				GSE_ERROR( EC.INVALID_DEREFERENCE, "Invalid range - opening index is behind closing index ( " + std::to_string( from.value() ) + " > " + std::to_string( to.value() ) + " )" );
			}
		}
	}
}

void Interpreter::CheckBreakCondition( gse::Value* const result, bool* need_break, bool* need_clear ) const {
	if ( !result ) {
		*need_break = false;
		*need_clear = false;
		return;
	}
	switch ( result->type ) {
		case gse::Value::T_LOOPCONTROL: {
			*need_clear = true;
			const auto type = ( (value::LoopControl*)result )->value;
			switch ( type ) {
				case program::LCT_BREAK: {
					*need_break = true;
					return;
				}
				case program::LCT_CONTINUE: {
					*need_break = false;
					return;
				}
				default:
					THROW( "unexpected loop control type: " + std::to_string( type ) );
			}
		}
		default:
			// got something to return
			*need_break = true;
			*need_clear = false;
	}
}

Interpreter::Function::Function(
	gc::Space* const gc_space,
	const Interpreter* runner,
	context::Context* context,
	const std::vector< std::string >& parameters,
	const Program* const program
)
	: value::Callable( gc_space, context )
	, runner( runner )
	, context( context )
	, parameters( parameters )
	, program( program ) {
}

Interpreter::Function::~Function() {
	delete program;
}

void Interpreter::Function::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	value::Callable::GetReachableObjects( reachable_objects );

	std::lock_guard guard( m_gc_mutex );

	GC_DEBUG_BEGIN( "Function" );

	ASSERT_NOLOG( context, "function ctx not set" );
	GC_DEBUG_BEGIN( "owner_context" );
	GC_REACHABLE( context );
	GC_DEBUG_END();

	GC_DEBUG_END();
}

gse::Value* Interpreter::Function::Run( GSE_CALLABLE, const function_arguments_t& arguments ) {
	if ( parameters.size() != arguments.size() ) {
		GSE_ERROR( EC.INVALID_CALL, "Expected " + std::to_string( parameters.size() ) + " arguments, found " + std::to_string( arguments.size() ) );
	}
	gse::Value* result = nullptr;
	context->ForkAndExecute(
		GSE_CALL, true, [ this, &arguments, &si, &ep, &result ]( context::ChildContext* const subctx ) {
			for ( size_t i = 0 ; i < parameters.size() ; i++ ) { // inject passed arguments
				subctx->CreateVariable( parameters[ i ], arguments[ i ], si, ep );
			}
			ep.WithSI(
				si, [ this, &result, &subctx, &ep ]() {
					result = runner->Execute( subctx, ep, program );
				}
			);
		}
	);
	return result
		? result
		: VALUE( Undefined ); // functions return undefined by default
}

}
}
