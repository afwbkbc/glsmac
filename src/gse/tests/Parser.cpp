#include "Tests.h"

#include "task/gsetests/GSETests.h"
#include "gc/Space.h"

#include "gse/GSE.h"
#include "gse/program/Program.h"
#include "gse/program/Scope.h"
#include "gse/program/Statement.h"
#include "gse/program/Conditional.h"
#include "gse/program/Expression.h"
#include "gse/program/Condition.h"
#include "gse/program/SimpleCondition.h"
#include "gse/program/Operator.h"
#include "gse/program/Value.h"
#include "gse/program/Variable.h"
#include "gse/program/Array.h"
#include "gse/program/Object.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"
#include "gse/program/If.h"
#include "gse/program/ElseIf.h"
#include "gse/program/Else.h"
#include "gse/program/While.h"
#include "gse/program/Try.h"
#include "gse/program/Catch.h"
#include "gse/parser/JS.h"

namespace gse {
using namespace program;
namespace tests {

void AddParserTests( task::gsetests::GSETests* task ) {

	const auto validate_program = []( const Program* program, const Program* reference_program ) -> std::string {
		GT_ASSERT( program != nullptr, "parser returned null program" );

#define VALIDATOR( _type, ... ) [ __VA_ARGS__ ]( const _type* a, const _type* b ) -> std::string
#define VALIDATOR_FORWARD_DEF( _var, _type ) std::function< std::string( const _type* a, const _type* b ) > _var;
#define VALIDATE( _func, _a, _b ) \
        errmsg = _func( (_a), (_b) ); \
        if (!errmsg.empty()) return errmsg;

		std::string errmsg = "";

		VALIDATOR_FORWARD_DEF( scope, Scope );
		VALIDATOR_FORWARD_DEF( expression, Expression );
		VALIDATOR_FORWARD_DEF( operand, Operand );

		const auto si = []( const si_t& a, const si_t& b ) -> std::string {
			if ( !a.file.empty() ) { // skip checks
				GT_ASSERT( a.file == b.file, "si filename differs ( " + a.file + " != " + b.file + " )" );
				GT_ASSERT( a.from.line == b.from.line && a.from.col == b.from.col, "si from differs ( " + std::to_string( a.from.line ) + ":" + std::to_string( a.from.col ) + " != " + std::to_string( b.from.line ) + ":" + std::to_string( b.from.col ) + " )" );
				GT_ASSERT( a.to.line == b.to.line && a.to.col == b.to.col, "si to differs ( " + std::to_string( a.to.line ) + ":" + std::to_string( a.to.col ) + " != " + std::to_string( b.to.line ) + ":" + std::to_string( b.to.col ) + " )" );
			}
			GT_OK();
		};

		const auto value = VALIDATOR( program::Value, &errmsg, &si ) {
			VALIDATE( si, a->m_si, b->m_si );
			GT_ASSERT( *a->value == *b->value, "values differ ( " + a->value->ToString() + " != " + b->value->ToString() + " )" );
			GT_OK();
		};

		const auto variable = VALIDATOR( Variable, &errmsg, &si ) {
			VALIDATE( si, a->m_si, b->m_si );
			GT_ASSERT( a->name == b->name, "variables names differ ( \"" + a->name + "\" != \"" + b->name + "\" )" );
			GT_ASSERT( a->hints == b->hints, "variables hints differ ( \"" + std::to_string( a->hints ) + "\" != \"" + std::to_string( b->hints ) + "\" )" );
			GT_OK();
		};

		const auto array = VALIDATOR( Array, &errmsg, &expression, &si ) {
			VALIDATE( si, a->m_si, b->m_si );
			GT_ASSERT( a->elements.size() == b->elements.size(), "arrays have different sizes ( " + std::to_string( a->elements.size() ) + " != " + std::to_string( b->elements.size() ) + " )" );

			for ( size_t i = 0 ; i < a->elements.size() ; i++ ) {
				VALIDATE( expression, a->elements[ i ], b->elements[ i ] );
			}

			GT_OK();
		};

		const auto object = VALIDATOR( Object, &errmsg, &expression, &si ) {
			VALIDATE( si, a->m_si, b->m_si );
			for ( const auto& it : a->properties ) {
				GT_ASSERT( b->properties.find( it.first ) != b->properties.end(), "property \"" + it.first + "\" exists in A but not B" );
			}
			for ( const auto& it : b->properties ) {
				GT_ASSERT( a->properties.find( it.first ) != a->properties.end(), "property \"" + it.first + "\" exists in B but not A" );
			}
			for ( const auto& it : a->properties ) {
				VALIDATE( expression, it.second, b->properties.at( it.first ) );
			}
			GT_OK();
		};

		const auto function = VALIDATOR( Function, &errmsg, &scope, &variable, &si ) {
			VALIDATE( si, a->m_si, b->m_si );
			GT_ASSERT( a->parameters.size() == b->parameters.size(), "parameters sizes differ ( " + std::to_string( a->parameters.size() ) + " != " + std::to_string( b->parameters.size() ) + " )" );
			for ( size_t i = 0 ; i < a->parameters.size() ; i++ ) {
				VALIDATE( variable, a->parameters[ i ], b->parameters[ i ] );
			}
			VALIDATE( scope, a->body, b->body );
			GT_OK();
		};

		const auto call = VALIDATOR( Call, &errmsg, &expression, &operand, &si ) {
			VALIDATE( si, a->m_si, b->m_si );
			VALIDATE( expression, a->callable, b->callable );
			GT_ASSERT( a->arguments.size() == b->arguments.size(), "arguments sizes differ ( " + std::to_string( a->arguments.size() ) + " != " + std::to_string( b->arguments.size() ) + " )" );
			for ( size_t i = 0 ; i < a->arguments.size() ; i++ ) {
				VALIDATE( operand, a->arguments[ i ], b->arguments[ i ] );
			}
			GT_OK();
		};

		operand = VALIDATOR( Operand,
			&errmsg,
			&value,
			&variable,
			&array,
			&object,
			&scope,
			&expression,
			&function,
			&call,
			&si
		) {
			GT_ASSERT( ( a == nullptr ) == ( b == nullptr ), "operands have different null states ( " + ( a == nullptr
				? "null"
				: a->ToString()
			) + " != " + ( b == nullptr
				? "null"
				: b->ToString()
			) + " )" );
			if ( a && b ) {
				VALIDATE( si, a->m_si, b->m_si );
				GT_ASSERT( a->type == b->type, "operands have different types ( " + a->ToString() + " != " + b->ToString() + " )" );
				switch ( a->type ) {
					case Operand::OT_NOTHING: {
						break;
					}
					case Operand::OT_VALUE: {
						VALIDATE( value, (program::Value*)a, (program::Value*)b );
						break;
					}
					case Operand::OT_VARIABLE: {
						VALIDATE( variable, (Variable*)a, (Variable*)b );
						break;
					}
					case Operand::OT_ARRAY: {
						VALIDATE( array, (Array*)a, (Array*)b );
						break;
					}
					case Operand::OT_OBJECT: {
						VALIDATE( object, (Object*)a, (Object*)b );
						break;
					}
					case Operand::OT_SCOPE: {
						VALIDATE( scope, (Scope*)a, (Scope*)b );
						break;
					}
					case Operand::OT_EXPRESSION: {
						VALIDATE( expression, (Expression*)a, (Expression*)b );
						break;
					}
					case Operand::OT_FUNCTION: {
						VALIDATE( function, (Function*)a, (Function*)b );
						break;
					}
					case Operand::OT_CALL: {
						VALIDATE( call, (Call*)a, (Call*)b );
						break;
					}
					default: {
						GT_FAIL( "unknown operand type: " + std::to_string( a->type ) );
					}
				}
			}
			GT_OK();
		};
		const auto operatr = VALIDATOR( Operator, &errmsg, &si ) {
			GT_ASSERT(
				( a == nullptr ) == ( b == nullptr ), "operators have different null states ( " + ( a == nullptr
				? "null"
				: a->Dump()
			) + " != " + ( b == nullptr
				? "null"
				: b->Dump()
			) + " )"
			);
			if ( a && b ) {
				VALIDATE( si, a->m_si, b->m_si );
				GT_ASSERT( a->op == b->op, "operators are different ( " + std::to_string( a->op ) + " != " + std::to_string( b->op ) + " )" );
			}
			GT_OK();
		};
		expression = VALIDATOR( Expression, &errmsg, &operand, &operatr, &si ) {
			VALIDATE( si, a->m_si, b->m_si );
			VALIDATE( operand, a->a, b->a );
			VALIDATE( operatr, a->op, b->op );
			VALIDATE( operand, a->b, b->b );
			GT_OK();
		};
		const auto simple_condition = VALIDATOR( SimpleCondition, &errmsg, &expression, &si ) {
			VALIDATE( si, a->m_si, b->m_si );
			VALIDATE( expression, a->expression, b->expression );
			GT_OK();
		};
		const auto condition = VALIDATOR( Condition, &errmsg, &simple_condition, &si ) {
			GT_ASSERT( a->type == b->type, "conditions have different types ( " + a->ToString() + " != " + b->ToString() + " )" );
			VALIDATE( si, a->m_si, b->m_si );
			switch ( a->type ) {
				case Condition::CT_SIMPLE: {
					VALIDATE( simple_condition, (SimpleCondition*)a, (SimpleCondition*)b );
					break;
				}
				default: {
					GT_FAIL( "unknown condition type: " + std::to_string( a->type ) );
				}
			}
			GT_OK();
		};
		const auto statement = VALIDATOR( Statement, &errmsg, &expression, &si ) {
			VALIDATE( si, a->m_si, b->m_si );
			VALIDATE( expression, a->body, b->body );
			GT_OK();
		};
		const std::function<
			std::string(
				const Conditional* a,
				const Conditional* b
			)
		> conditional = VALIDATOR(
			Conditional,
			&errmsg,
			&condition,
			&scope,
			&conditional,
			&object,
			&si
		) {
			VALIDATE( si, a->m_si, b->m_si );
			GT_ASSERT( a->conditional_type == b->conditional_type, "conditionals have different types ( " + a->Dump() + " != " + b->Dump() + " )" );
			switch ( a->conditional_type ) {
				case Conditional::CT_IF: {
					VALIDATE( condition, ( (If*)a )->condition, ( (If*)b )->condition );
					VALIDATE( scope, ( (If*)a )->body, ( (If*)b )->body );
					break;
				}
				case Conditional::CT_ELSEIF: {
					VALIDATE( condition, ( (ElseIf*)a )->condition, ( (ElseIf*)b )->condition );
					VALIDATE( scope, ( (ElseIf*)a )->body, ( (ElseIf*)b )->body );
					break;
				}
				case Conditional::CT_ELSE: {
					VALIDATE( scope, ( (Else*)a )->body, ( (Else*)b )->body );
					break;
				}
				case Conditional::CT_WHILE: {
					VALIDATE( condition, ( (While*)a )->condition, ( (While*)b )->condition );
					VALIDATE( scope, ( (While*)a )->body, ( (While*)b )->body );
					break;
				}
				case Conditional::CT_FOR: {
					THROW( "TODO: PARSER FOR" );
				}
				case Conditional::CT_TRY: {
					VALIDATE( scope, ( (Try*)a )->body, ( (Try*)b )->body );
					VALIDATE( conditional, ( (Try*)a )->handlers, ( (Try*)b )->handlers );
					break;
				}
				case Conditional::CT_CATCH: {
					VALIDATE( object, ( (Catch*)a )->handlers, ( (Catch*)b )->handlers );
					break;
				}
				default: {
					GT_FAIL( "unknown conditional type: " + std::to_string( a->conditional_type ) );
				}
			}
			GT_OK();
		};
		const auto control = VALIDATOR( Control, &errmsg, &statement, &conditional, &si ) {
			GT_ASSERT( a->control_type == b->control_type, "controls have different types ( " + a->Dump() + " != " + b->Dump() + " )" );
			VALIDATE( si, a->m_si, b->m_si );
			switch ( a->control_type ) {
				case Control::CT_CONDITIONAL: {
					VALIDATE( conditional, (Conditional*)a, (Conditional*)b );
					break;
				}
				case Control::CT_STATEMENT: {
					VALIDATE( statement, (Statement*)a, (Statement*)b );
					break;
				}
				default: {
					GT_FAIL( "unknown control type: " + std::to_string( a->control_type ) );
				}
			}
			GT_OK();
		};
		scope = VALIDATOR( Scope, &errmsg, &control, &si ) {
			VALIDATE( si, a->m_si, b->m_si );
			GT_ASSERT( a->body.size() == b->body.size(), "scope sizes differ ( " + std::to_string( a->body.size() ) + " != " + std::to_string( b->body.size() ) + " )" );
			for ( size_t i = 0 ; i < a->body.size() ; i++ ) {
				VALIDATE( control, a->body[ i ], b->body[ i ] );
			}
			GT_OK();
		};

		VALIDATE( scope, program->body, reference_program->body );

#undef VALIDATOR
#undef VALIDATE
		GT_OK();
	};

	task->AddTest(
		"test if JS parser produces valid output",
		GT( validate_program ) {
			auto* gc_space = gse->GetGCSpace();
			std::string result = "";
			gc_space->Accumulate(
				[ &gc_space, &validate_program, &result ]() {
					NEWV( parser, parser::JS, gc_space, GetTestFilename(), GetTestSource(), 1 );
					const auto* program = parser->Parse();
					const auto* reference_program = gse::tests::GetTestProgram( gc_space );
					ASSERT( reference_program, "reference program is null" );
					result = validate_program( reference_program, program );
					if ( program ) {
						DELETE( program );
					}
					delete reference_program;
				}
			);
			return result;
		}
	);

}

}
}
