#include "GSE.h"

#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"

#include "gse/program/Program.h"
#include "gse/program/Scope.h"
#include "gse/program/Statement.h"
#include "gse/program/Expression.h"
#include "gse/program/Operator.h"
#include "gse/program/Nothing.h"
#include "gse/program/Value.h"
#include "gse/program/Variable.h"
#include "gse/program/Object.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"

#include "gse/parser/GJS.h"

namespace gse {
namespace tests {

using namespace program;

void AddParserTests( task::gsetests::GSETests* task ) {

	// we will compare parsers' outputs to this program
	static const Program reference_program(
		new Scope(
			{
				new Statement(
					new Expression(
						new Variable( "a" ),
						new Operator( Operator::OT_ASSIGN ),
						new program::Value( VALUE( type::Int, 5 ) )
					)
				),
				new Statement(
					new Expression(
						new Variable( "a" ),
						new Operator( Operator::OT_INC ),
						new Nothing()
					)
				),
				new Statement(
					new Expression(
						new Variable( "b" ),
						new Operator( Operator::OT_ASSIGN ),
						new Expression(
							new Variable( "a" ),
							new Operator( Operator::OT_ADD ),
							new program::Value( VALUE( type::Int, 2 ) )
						)
					)
				),
				new Statement(
					new Expression(
						new Scope(
							{
								new Statement(
									new Expression(
										new Variable( "a" ),
										new Operator( Operator::OT_ASSIGN ),
										new program::Value( VALUE( type::Int, 15 ) )
									)
								),
								new Statement(
									new Expression(
										new Variable( "a" ),
										new Operator( Operator::OT_INCBY ),
										new program::Value( VALUE( type::Int, 15 ) )
									)
								)
							}
						)
					)
				),
				new Statement(
					new Expression(
						new Variable( "c" ),
						new Operator( Operator::OT_ASSIGN ),
						new program::Value( VALUE( type::Int, 123 ) )
					)
				),
				new Statement(
					new Expression(
						new Variable( "testmethod1" ),
						new Operator( Operator::OT_ASSIGN ),
						new Function(
							{
								new Variable( "a" ),
								new Variable( "b" ),
								new Variable( "c" )
							},
							new Scope(
								{
									new Statement(
										new Expression(
											new Nothing(),
											new Operator( Operator::OT_RETURN ),
											new Expression(
												new Expression(
													new Variable( "a" ),
													new Operator( Operator::OT_ADD ),
													new Variable( "b" )
												),
												new Operator( Operator::OT_ADD ),
												new Variable( "c" )
											)
										)
									)
								}
							)
						)
					)
				),
				new Statement(
					new Expression(
						new Variable( "testmethod2" ),
						new Operator( Operator::OT_ASSIGN ),
						new Function(
							{
								new Variable( "a" ),
								new Variable( "b" ),
								new Variable( "c" )
							},
							new Scope(
								{
									new Statement(
										new Expression(
											new Nothing(),
											new Operator( Operator::OT_RETURN ),
											new Expression(
												new Expression(
													new Variable( "a" ),
													new Operator( Operator::OT_ADD ),
													new Variable( "b" )
												),
												new Operator( Operator::OT_SUB ),
												new Variable( "c" )
											)
										)
									)
								}
							)
						)
					)
				),
				new Statement(
					new Expression(
						new Variable( "testobj1" ),
						new Operator( Operator::OT_ASSIGN ),
						new Object( {} )
					)
				),
				new Statement(
					new Expression(
						new Variable( "testobj2" ),
						new Operator( Operator::OT_ASSIGN ),
						new Object(
							{
								{
									"propertyString",
									new program::Value( VALUE( type::String, "STRING" ) )
								},
								{
									"propertyInt1",
									new Expression(
										new Expression(
											new program::Value( VALUE( type::Int, 111 ) ),
											new Operator( Operator::OT_ADD ),
											new Variable( "a" )
										),
										new Operator( Operator::OT_ADD ),
										new Variable( "b" )
									)
								},
								{
									"propertyInt2",
									new program::Value( VALUE( type::Int, 222 ) )
								}
							}
						)
					)
				),
				new Statement(
					new Expression(
						new Expression(
							new Variable( "testobj1" ),
							new Operator( Operator::OT_CHILD ),
							new Variable( "propertyInt" )
						),
						new Operator( Operator::OT_ASSIGN ),
						new Expression(
							new Expression(
								new Variable( "testobj2" ),
								new Operator( Operator::OT_CHILD ),
								new Variable( "propertyInt1" )
							),
							new Operator( Operator::OT_ADD ),
							new Expression(
								new Variable( "testobj2" ),
								new Operator( Operator::OT_CHILD ),
								new Variable( "propertyInt2" )
							)
						)
					)
				),
				new Statement(
					new Expression(
						new Call(
							new Expression(
								new Variable( "console" ),
								new Operator( Operator::OT_CHILD ),
								new Variable( "log" )
							),
							{
								new Expression(
									new Expression(
										new Variable( "testobj1" ),
										new Operator( Operator::OT_CHILD ),
										new Variable( "propertyInt" )
									),
									new Operator( Operator::OT_EQUALS ),
									new program::Value( VALUE( type::Int, 348 ) )
								)
							}
						)
					)
				),
				new Statement(
					new Expression(
						new Call(
							new Expression(
								new Variable( "console" ),
								new Operator( Operator::OT_CHILD ),
								new Variable( "log" )
							),
							{
								new Variable( "testobj1" ),
								new Variable( "testobj2" ),
							}
						)
					)
				),
				new Statement(
					new Expression(
						new Call(
							new Expression(
								new Variable( "console" ),
								new Operator( Operator::OT_CHILD ),
								new Variable( "log" )
							),
							{
								new program::Value( VALUE( type::String, "bye!" ) ),
							}
						)
					)
				),
			}
		)
	);

	static const auto validate_program = []( const Program* program ) -> std::string {
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
			GT_OK();
		};

		const auto object = VALIDATOR( Object, &errmsg, &operand ) {
			for ( const auto& it : a->properties ) {
				GT_ASSERT( b->properties.find( it.first ) != b->properties.end(), "property \"" + it.first + "\" exists in A but not B" );
			}
			for ( const auto& it : b->properties ) {
				GT_ASSERT( a->properties.find( it.first ) != a->properties.end(), "property \"" + it.first + "\" exists in B but not A" );
			}
			for ( const auto& it : a->properties ) {
				VALIDATE( operand, it.second, b->properties.at( it.first ) );
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
			&object,
			&scope,
			&expression,
			&function,
			&call
		) {
			GT_ASSERT( a->type == b->type, "operands have different types ( " + std::to_string( a->type ) + " != " + std::to_string( b->type ) + " )" );
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
			GT_OK();
		};
		const auto operatr = VALIDATOR( Operator ) {
			GT_ASSERT( a->op == b->op, "operators are different ( " + std::to_string( a->op ) + " != " + std::to_string( b->op ) + " )" );
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
		scope = VALIDATOR( Scope, &errmsg, &statement ) {
			GT_ASSERT( a->body.size() == b->body.size(), "scope sizes differ ( " + std::to_string( a->body.size() ) + " != " + std::to_string( b->body.size() ) + " )" );
			for ( size_t i = 0 ; i < a->body.size() ; i++ ) {
				VALIDATE( statement, a->body[ i ], b->body[ i ] );
			}
			GT_OK();
		};

		VALIDATE( scope, reference_program.body, program->body );

#undef VALIDATOR
#undef VALIDATE
		GT_OK();
	};

	task->AddTest(
		"test if GJS parser produces valid output",
		GT() {
			parser::GJS parser;
			const auto* program = parser.Parse(
				"gjs",
				""
				"// test script"
				""
				"a = 5;"
				"a++;"
				"b = a + 2;"
				"{"
				"	a = 15;"
				"	a += 10;"
				"};"
				"c = 123;"
				""
				"testmethod1 = (a, b, c) => { return a + b + c; };"
				""
				"testmethod2 = (a, b, c) => {"
				"	/*"
				"		this method is a bit different"
				"	*/"
				"	return"
				"		a"
				"			+"
				"		b"
				"			-"
				"		c"
				"	;"
				"};"
				""
				"testobj1 = {};"
				"testobj2 = {"
				"	propertyString: 'STRING',"
				"	propertyInt1: 111 + a + b,"
				"	propertyInt2: 222,"
				"};"
				"testobj1.propertyInt = testobj2.propertyInt1 + testobj2.propertyInt2;"
				""
				"console.log(testobj1.propertyInt == 348); console.log(testobj1, testobj2);"
				""
				"console.log('bye!');"
			);
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
