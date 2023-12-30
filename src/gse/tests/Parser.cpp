#include "GSE.h"

#include "gse/Tests.h"
#include "gse/type/Int.h"
#include "gse/program/Program.h"
#include "gse/program/Scope.h"
#include "gse/program/Statement.h"
#include "gse/program/Conditional.h"
#include "gse/program/Expression.h"
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
#include "gse/parser/GJS.h"

namespace gse {
namespace tests {

using namespace program;

void AddParserTests( task::gsetests::GSETests* task ) {

	const Program* reference_program = GetTestProgram();

	const auto validate_program = [ reference_program ]( const Program* program ) -> std::string {
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

		const auto value = VALIDATOR( program::Value ) {
			GT_ASSERT( a->value == b->value, "values differ ( " + a->value.ToString() + " != " + b->value.ToString() + " )" );
			GT_OK();
		};

		const auto variable = VALIDATOR( Variable ) {
			GT_ASSERT( a->name == b->name, "variables names differ ( \"" + a->name + "\" != \"" + b->name + "\" )" );
			GT_ASSERT( a->hints == b->hints, "variables hints differ ( \"" + std::to_string( a->hints ) + "\" != \"" + std::to_string( b->hints ) + "\" )" );
			GT_OK();
		};

		const auto array = VALIDATOR( Array, &errmsg, &expression ) {
			GT_ASSERT( a->elements.size() == b->elements.size(), "arrays have different sizes ( " + std::to_string( a->elements.size() ) + " != " + std::to_string( b->elements.size() ) + " )" );

			for ( size_t i = 0 ; i < a->elements.size() ; i++ ) {
				VALIDATE( expression, a->elements[ i ], b->elements[ i ] );
			}

			GT_OK();
		};

		const auto object = VALIDATOR( Object, &errmsg, &expression ) {
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

		const auto function = VALIDATOR( Function, &errmsg, &scope ) {
			GT_ASSERT( a->parameters.size() == b->parameters.size(), "parameters sizes differ ( " + std::to_string( a->parameters.size() ) + " != " + std::to_string( b->parameters.size() ) + " )" );
			VALIDATE( scope, a->body, b->body );
			GT_OK();
		};

		const auto call = VALIDATOR( Call, &errmsg, &expression, &operand ) {
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
			&call
		) {
			GT_ASSERT( ( a == nullptr ) == ( b == nullptr ), "operands have different null states ( " + ( a == nullptr
				? "null"
				: a->ToString()
			) + " != " + ( b == nullptr
				? "null"
				: b->ToString()
			) + " )" );
			if ( a && b ) {
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
						GT_ASSERT( false, "unknown operand type: " + std::to_string( a->type ) );
					}
				}
			}
			GT_OK();
		};
		const auto operatr = VALIDATOR( Operator ) {
			GT_ASSERT( ( a == nullptr ) == ( b == nullptr ), "operators have different null states ( " + ( a == nullptr
				? "null"
				: a->ToString()
			) + " != " + ( b == nullptr
				? "null"
				: b->ToString()
			) + " )" );
			if ( a && b ) {
				GT_ASSERT( a->op == b->op, "operators are different ( " + std::to_string( a->op ) + " != " + std::to_string( b->op ) + " )" );
			}
			GT_OK();
		};
		expression = VALIDATOR( Expression, &errmsg, &operand, &operatr ) {
			VALIDATE( operand, a->a, b->a );
			VALIDATE( operatr, a->op, b->op );
			VALIDATE( operand, a->b, b->b );
			GT_OK();
		};
		const auto statement = VALIDATOR( Statement, &errmsg, &expression ) {
			VALIDATE( expression, a->body, b->body );
			GT_OK();
		};
		const auto conditional = VALIDATOR( Conditional, &errmsg, &expression, &scope ) {
			GT_ASSERT( a->conditional_type == b->conditional_type, "conditionals have different types ( " + a->ToString() + " != " + b->ToString() + " )" );
			switch ( a->conditional_type ) {
				case Conditional::CT_IF: {
					VALIDATE( expression, ( (If*)a )->condition, ( (If*)b )->condition );
					VALIDATE( scope, ( (If*)a )->body, ( (If*)b )->body );
					break;
				}
				case Conditional::CT_ELSEIF: {
					VALIDATE( expression, ( (ElseIf*)a )->condition, ( (ElseIf*)b )->condition );
					VALIDATE( scope, ( (ElseIf*)a )->body, ( (ElseIf*)b )->body );
					break;
				}
				case Conditional::CT_ELSE: {
					VALIDATE( scope, ( (Else*)a )->body, ( (Else*)b )->body );
					break;
				}
				case Conditional::CT_WHILE: {
					VALIDATE( expression, ( (While*)a )->condition, ( (While*)b )->condition );
					VALIDATE( scope, ( (While*)a )->body, ( (While*)b )->body );
					break;
				}
				default: {
					GT_ASSERT( false, "unknown conditional type: " + std::to_string( a->conditional_type ) );
				}
			}
			GT_OK();
		};
		const auto control = VALIDATOR( Control, &errmsg, &statement, &conditional ) {
			GT_ASSERT( a->control_type == b->control_type, "controls have different types ( " + a->ToString() + " != " + b->ToString() + " )" );
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
					GT_ASSERT( false, "unknown control type: " + std::to_string( a->control_type ) );
				}
			}
			GT_OK();
		};
		scope = VALIDATOR( Scope, &errmsg, &control ) {
			GT_ASSERT( a->body.size() == b->body.size(), "scope sizes differ ( " + std::to_string( a->body.size() ) + " != " + std::to_string( b->body.size() ) + " )" );
			for ( size_t i = 0 ; i < a->body.size() ; i++ ) {
				VALIDATE( control, a->body[ i ], b->body[ i ] );
			}
			GT_OK();
		};

		VALIDATE( scope, reference_program->body, program->body );

#undef VALIDATOR
#undef VALIDATE
		GT_OK();
	};

	task->AddTest(
		"test if GJS parser produces valid output",
		GT( validate_program ) {
			parser::GJS parser(
				""
				"// test script\n"
				"\n"
				"let a = 5;\n"
				"a++;\n"
				"let b = a + 2 * 4;\n"
				"let c=(a+2)*4;\n"
				"{\n"
				"	a = 15;\n"
				"	a += 10;\n"
				"};\n"
				"c = 123;\n"
				"c -= 23;\n"
				"\n"
				"let testmethod1 = (a, b, c) => { return a + b + c; };\n"
				"\n"
				"let testmethod2 = (a, b, c) => {\n"
				"	/*\n"
				"		this method is a bit different\n"
				"	*/\n"
				"	return\n"
				"		a\n"
				"			+\n"
				"		b\n"
				"			-\n"
				"		c\n"
				"	;\n"
				"};\n"
				"\n"
				"let testarr1 = [];\n"
				"let testarr2 = [ 3, 'TEST', {\n"
				"  key1: 'value1',\n"
				"  key2: 'value2',\n"
				"} ];\n"
				"testarr1 []= 'first';\n"
				"testarr1 []= 'second';\n"
				"testarr1 []= 1 + 2 + 3;\n"
				"testarr1 += testarr2;\n"
				"testarr1 []= testarr2;\n"
				"let testarr3 = testarr1;\n"
				"testarr3[1] = 'SECOND';\n"
				"testarr3[ testmethod2(a, b, c) + 61 ] = 'FIRST';\n"
				"testarr3[2:5] = testarr1[0:1] + testarr2[0:1];\n"
				"let testarr4 = testarr3[:3];\n"
				"testarr4[ c + 1 - 100 : c - 100 + 2 ] = [ 'new first', 'new second' ];\n"
				"\n"
				"let testobj1 = {};\n"
				"let testobj2 = {\n"
				"	propertyString: 'STRING',\n"
				"	propertyInt1: 111 + a + b,\n"
				"	propertyInt2: 222,\n"
				"};\n"
				"let testobj3 = {\n"
				"  child1: {\n"
				"    child2: {\n"
				"      value: 'CHILD VALUE'\n"
				"    }\n"
				"  },\n"
				"};\n"
				"testobj1.propertyInt = testobj2.propertyInt1 + testobj2.propertyInt2;\n"
				"\n"
				"let d = null;\n"
				"let x = a > b;\n"
				"\n"
				"console.log( d );\n"
				"console.log( d == null );\n"
				"console.log( x, x == b > c );\n"
				"\n"
				"console.log( a != b, b != c, c != a, a != a );\n"
				"console.log( a > b, b > c );\n"
				"console.log( b >= a, a >= 2, c <= 200, a <= 200 );\n"
				"console.log( 10 < 10, 10 <= 10, a < a, a <= a );\n"
				"console.log( true && true, true && false, true || true, true || false );\n"
				"console.log( (( 5 > 10 ) && ( 2 > 1 )) || (( 5 <= 10 ) && !( 5 > 35 ) && ( 100 >= 20 )) );\n"
				"console.log(testmethod1(11, b, 20), testmethod2(a, b, c));\n"
				"let testmethod = testmethod1;\n"
				"console.log( testmethod( 1, testmethod( 2, testmethod( 3, 3, 3 ), testmethod( 4, 4, 4 ) ), testmethod( 5, 5, testmethod( 6, 6, 6 )) ), 10 );\n"
				"console.log( testarr1 ); console.log( testarr2 ); console.log( testarr3 ); console.log( testarr4 );\n"
				"console.log( testarr1[0] ); console.log( testarr1[1] ); console.log( testarr1[0:1] );\n"
				"console.log( testarr1[5:] ); console.log( testarr1[:3] );\n"
				"console.log( testarr1[4:5] + testarr1[2:3] );\n"
				"console.log(testobj3.child1.child2.value);\n"
				"console.log(testobj1.propertyInt == 272 + c); console.log(testobj1, testobj2);\n"
				"\n"
				"if ( a > b ) {\n"
				"  console.log( 'YES' );\n"
				"}\n"
				"else {\n"
				"  console.log( 'NO' );\n"
				"};\n"
				"if ( b > a ) {\n"
				"  console.log( 'YES' );\n"
				"}\n"
				"else {\n"
				"  console.log( 'NO' );\n"
				"};\n"
				"if ( false ) { console.log( 'FALSE' ); };\n"
				"if ( false ) {\n"
				"  console.log('FAIL');\n"
				"} elseif ( false ) {\n"
				"  console.log( 'FAIL' );\n"
				"} elseif ( true ) {\n"
				"  console.log( 'OK' );\n"
				"} else {\n"
				"  console.log( 'FAIL' );\n"
				"};\n"
				"\n"
				"let i = 0;\n"
				"while ( i++ < 5 ) {\n"
				"  console.log(i);\n"
				"};\n"
				"\n"
				";;;\n"
				"console.log('bye!');\n"
			);
			const auto* program = parser.Parse();
			//GT_FAIL( program->ToString() );
			const auto result = validate_program( program );
			if ( program ) {
				DELETE( program );
			}
			return result;
		}
	);

}

}
}
