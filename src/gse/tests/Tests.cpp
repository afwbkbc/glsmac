#include "Tests.h"

#include "GSE.h"
#include "Parser.h"
#include "Runner.h"
#include "Scripts.h"

#include "engine/Engine.h"
#include "config/Config.h"
#include "task/gsetests/GSETests.h"

#include "gse/program/Program.h"
#include "gse/program/Variable.h"
#include "gse/program/Value.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"
#include "gse/program/Array.h"
#include "gse/program/If.h"
#include "gse/program/ElseIf.h"
#include "gse/program/Else.h"
#include "gse/program/While.h"
#include "gse/program/Try.h"
#include "gse/program/Catch.h"
#include "gse/program/Statement.h"
#include "gse/program/Expression.h"
#include "gse/program/Operator.h"
#include "gse/program/Object.h"
#include "gse/type/Bool.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/type/Null.h"
#include "gse/type/Range.h"

namespace gse {
namespace tests {

void AddTests( task::gsetests::GSETests* task ) {

	if ( !g_engine->GetConfig()->HasDebugFlag( config::Config::DF_GSE_TESTS_SCRIPT ) ) {
		task->AddTest(
			"test if tests work",
			GT() {
				GT_OK();
			}
		);
		tests::AddGSETests( task );
		tests::AddParserTests( task );
		tests::AddRunnerTests( task );
	}
	tests::AddScriptsTests( task );

}

const std::string& GetTestFilename() {
	const static std::string filename = "<TEST>.gls.js";
	return filename;
}

const std::string& GetTestSource() {
	const static std::string source = ""
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
									  "     #print( d );\n"
									  "     #print( d == null );\n"
									  "     #print( x, x == b > c );\n"
									  "\n"
									  "     #print( a != b, b != c, c != a, a != a );\n"
									  "     #print( a > b, b > c );\n"
									  "     #print( b >= a, a >= 2, c <= 200, a <= 200 );\n"
									  "     #print( 10 < 10, 10 <= 10, a < a, a <= a );\n"
									  "     #print( true && true, true && false, true || true, true || false );\n"
									  "     #print( (( 5 > 10 ) && ( 2 > 1 )) || (( 5 <= 10 ) && !( 5 > 35 ) && ( 100 >= 20 )) );\n"
									  "     #print(testmethod1(11, b, 20), testmethod2(a, b, c));\n"
									  "let testmethod = testmethod1;\n"
									  "     #print( testmethod( 1, testmethod( 2, testmethod( 3, 3, 3 ), testmethod( 4, 4, 4 ) ), testmethod( 5, 5, testmethod( 6, 6, 6 )) ), 10 );\n"
									  "     #print( testarr1 );      #print( testarr2 );      #print( testarr3 );      #print( testarr4 );\n"
									  "     #print( testarr1[0] );      #print( testarr1[1] );      #print( testarr1[0:1] );\n"
									  "     #print( testarr1[5:] );      #print( testarr1[:3] );\n"
									  "     #print( testarr1[4:5] + testarr1[2:3] );\n"
									  "     #print(testobj3.child1.child2.value);\n"
									  "     #print(testobj1.propertyInt == 272 + c);      #print(testobj1, testobj2);\n"
									  "\n"
									  "if ( a > b ) {\n"
									  "       #print( 'YES' );\n"
									  "}\n"
									  "else {\n"
									  "       #print( 'NO' );\n"
									  "};\n"
									  "if ( b > a ) {\n"
									  "       #print( 'YES' );\n"
									  "}\n"
									  "else {\n"
									  "       #print( 'NO' );\n"
									  "};\n"
									  "if ( false ) {      #print( 'FALSE' ); };\n"
									  "if ( false ) {\n"
									  "       #print('FAIL');\n"
									  "} elseif ( false ) {\n"
									  "       #print( 'FAIL' );\n"
									  "} elseif ( true ) {\n"
									  "       #print( 'OK' );\n"
									  "} else {\n"
									  "       #print( 'FAIL' );\n"
									  "};\n"
									  "\n"
									  "let i = 0;\n"
									  "while ( i++ < 5 ) {\n"
									  "       #print(i);\n"
									  "};\n"
									  "\n"
									  "try {\n"
									  "       #print( 'BEFORE EXCEPTION' ); // should be printed\n"
									  "  let failfunc = () => {\n"
									  "         #print('failfunc');\n"
									  "    throw TestError('something happened');\n"
									  "  };\n"
									  "  failfunc();\n"
									  "       #print( 'AFTER EXCEPTION' ); // should not be printed\n"
									  "}\n"
									  "catch {\n"
									  "  UnknownError: (e) => {\n"
									  "         #print('shouldnt catch this');\n"
									  "  },\n"
									  "  TestError: (e) => {\n"
									  "         #print('CAUGHT ' + e.type + ' : ' + e.reason);\n"
									  "         #print(e.backtrace);\n"
									  "  }\n"
									  "};\n"
									  "\n"
									  ";;;\n"
									  "     #print('bye!');\n";
	return source;
}

using namespace program;

const Program* GetTestProgram() {
	const auto& filename = GetTestFilename();
// TODO: other parsers will need different values here
#define SI( _fromline, _fromcol, _toline, _tocol ) { \
    filename, \
    { \
        _fromline, \
            _fromcol \
    }, \
    { \
        _toline, \
            _tocol \
    } \
}

// skip validation in some places
#define SI_SKIP() { "", { 0, 0 }, { 0, 0 } }

	const auto print = [ &filename ]( const size_t line, size_t col_begin, const std::vector< const Expression* >& arguments ) -> Statement* {
		return new Statement(
			SI_SKIP(),
			new Expression(
				SI_SKIP(),
				new Call(
					SI_SKIP(),
					new Expression(
						SI( line, col_begin, line, col_begin + 6 ),
						new Variable( SI( line, col_begin, line, col_begin + 6 ), "#print" )
					),
					arguments
				)
			)
		);
	};

	static const auto* program = new Program(
		new Scope(
			SI( 3, 1, 132, 21 ),
			{
				new Statement(
					SI( 3, 1, 3, 10 ),
					new Expression(
						SI( 3, 5, 3, 10 ),
						new Variable( SI( 3, 5, 3, 6 ), "a", VH_CREATE_VAR ),
						new Operator( SI( 3, 7, 3, 8 ), OT_ASSIGN ),
						new program::Value( SI( 3, 9, 3, 10 ), VALUE( type::Int, 5 ) )
					)
				),
				new Statement(
					SI( 4, 1, 4, 4 ),
					new Expression(
						SI( 4, 1, 4, 4 ),
						new Variable( SI( 4, 1, 4, 2 ), "a" ),
						new Operator( SI( 4, 2, 4, 4 ), OT_INC )
					)
				),
				new Statement(
					SI( 5, 1, 5, 18 ),
					new Expression(
						SI( 5, 5, 5, 18 ),
						new Variable( SI( 5, 5, 5, 6 ), "b", VH_CREATE_VAR ),
						new Operator( SI( 5, 7, 5, 8 ), OT_ASSIGN ),
						new Expression(
							SI( 5, 9, 5, 18 ),
							new Variable( SI( 5, 9, 5, 10 ), "a" ),
							new Operator( SI( 5, 11, 5, 12 ), OT_ADD ),
							new Expression(
								SI( 5, 13, 5, 18 ),
								new program::Value( SI( 5, 13, 5, 14 ), VALUE( type::Int, 2 ) ),
								new Operator( SI( 5, 15, 5, 16 ), OT_MULT ),
								new program::Value( SI( 5, 17, 5, 18 ), VALUE( type::Int, 4 ) )
							)
						)
					)
				),
				new Statement(
					SI( 6, 1, 6, 14 ),
					new Expression(
						SI( 6, 5, 6, 14 ),
						new Variable( SI( 6, 5, 6, 6 ), "c", VH_CREATE_VAR ),
						new Operator( SI( 6, 6, 6, 7 ), OT_ASSIGN ),
						new Expression(
							SI( 6, 8, 6, 14 ),
							new Expression(
								SI( 6, 8, 6, 11 ),
								new Variable( SI( 6, 8, 6, 9 ), "a" ),
								new Operator( SI( 6, 9, 6, 10 ), OT_ADD ),
								new program::Value( SI( 6, 10, 6, 11 ), VALUE( type::Int, 2 ) )
							),
							new Operator( SI( 6, 12, 6, 13 ), OT_MULT ),
							new program::Value( SI( 6, 13, 6, 14 ), VALUE( type::Int, 4 ) )
						)
					)
				),
				new Statement(
					SI( 7, 1, 10, 2 ),
					new Expression(
						SI( 8, 2, 9, 10 ),
						new Scope(
							SI( 8, 2, 9, 10 ),
							{
								new Statement(
									SI( 8, 2, 8, 8 ),
									new Expression(
										SI( 8, 2, 8, 8 ),
										new Variable( SI( 8, 2, 8, 3 ), "a" ),
										new Operator( SI( 8, 4, 8, 5 ), OT_ASSIGN ),
										new program::Value( SI( 8, 6, 8, 8 ), VALUE( type::Int, 15 ) )
									)
								),
								new Statement(
									SI( 9, 2, 9, 9 ),
									new Expression(
										SI( 9, 2, 9, 9 ),
										new Variable( SI( 9, 2, 9, 3 ), "a" ),
										new Operator( SI( 9, 4, 9, 6 ), OT_INC_BY ),
										new program::Value( SI( 9, 7, 9, 9 ), VALUE( type::Int, 10 ) )
									)
								)
							}
						)
					)
				),
				new Statement(
					SI( 11, 1, 11, 8 ),
					new Expression(
						SI( 11, 1, 11, 8 ),
						new Variable( SI( 11, 1, 11, 2 ), "c" ),
						new Operator( SI( 11, 3, 11, 4 ), OT_ASSIGN ),
						new program::Value( SI( 11, 5, 11, 8 ), VALUE( type::Int, 123 ) )
					)
				),
				new Statement(
					SI( 12, 1, 12, 8 ),
					new Expression(
						SI( 12, 1, 12, 8 ),
						new Variable( SI( 12, 1, 12, 2 ), "c" ),
						new Operator( SI( 12, 3, 12, 5 ), OT_DEC_BY ),
						new program::Value( SI( 12, 6, 12, 8 ), VALUE( type::Int, 23 ) )
					)
				),
				new Statement(
					SI( 14, 1, 14, 53 ),
					new Expression(
						SI( 14, 5, 14, 53 ),
						new Variable( SI( 14, 5, 14, 16 ), "testmethod1", VH_CREATE_VAR ),
						new Operator( SI( 14, 17, 14, 18 ), OT_ASSIGN ),
						new Function(
							SI( 14, 19, 14, 53 ),
							{
								new Variable( SI( 14, 20, 14, 21 ), "a" ),
								new Variable( SI( 14, 23, 14, 24 ), "b" ),
								new Variable( SI( 14, 26, 14, 27 ), "c" )
							},
							new Scope(
								SI( 14, 34, 14, 51 ),
								{
									new Statement(
										SI( 14, 34, 14, 50 ),
										new Expression(
											SI( 14, 34, 14, 50 ),
											nullptr,
											new Operator( SI( 14, 34, 14, 40 ), OT_RETURN ),
											new Expression(
												SI( 14, 41, 14, 50 ),
												new Expression(
													SI( 14, 41, 14, 46 ),
													new Variable( SI( 14, 41, 14, 42 ), "a" ),
													new Operator( SI( 14, 43, 14, 44 ), OT_ADD ),
													new Variable( SI( 14, 45, 14, 46 ), "b" )
												),
												new Operator( SI( 14, 47, 14, 48 ), OT_ADD ),
												new Variable( SI( 14, 49, 14, 50 ), "c" )
											)
										)
									)
								}
							)
						)
					)
				),
				new Statement(
					SI( 16, 1, 27, 2 ),
					new Expression(
						SI( 16, 5, 27, 2 ),
						new Variable( SI( 16, 5, 16, 16 ), "testmethod2", VH_CREATE_VAR ),
						new Operator( SI( 16, 17, 16, 18 ), OT_ASSIGN ),
						new Function(
							SI( 16, 19, 27, 2 ),
							{
								new Variable( SI( 16, 20, 16, 21 ), "a" ),
								new Variable( SI( 16, 23, 16, 24 ), "b" ),
								new Variable( SI( 16, 26, 16, 27 ), "c" )
							},
							new Scope(
								SI( 20, 2, 26, 3 ),
								{
									new Statement(
										SI( 20, 2, 25, 4 ),
										new Expression(
											SI( 20, 2, 25, 4 ),
											nullptr,
											new Operator( SI( 20, 2, 20, 8 ), OT_RETURN ),
											new Expression(
												SI( 21, 3, 25, 4 ),
												new Expression(
													SI( 21, 3, 23, 4 ),
													new Variable( SI( 21, 3, 21, 4 ), "a" ),
													new Operator( SI( 22, 4, 22, 5 ), OT_ADD ),
													new Variable( SI( 23, 3, 23, 4 ), "b" )
												),
												new Operator( SI( 24, 4, 24, 5 ), OT_SUB ),
												new Variable( SI( 25, 3, 25, 4 ), "c" )
											)
										)
									)
								}
							)
						)
					)
				),
				new Statement(
					SI( 29, 1, 29, 18 ),
					new Expression(
						SI( 29, 5, 29, 18 ),
						new Variable( SI( 29, 5, 29, 13 ), "testarr1", VH_CREATE_VAR ),
						new Operator( SI( 29, 14, 29, 15 ), OT_ASSIGN ),
						new Array( SI( 29, 16, 29, 18 ), {} )
					)
				),
				new Statement(
					SI( 30, 1, 33, 4 ),
					new Expression(
						SI( 30, 5, 33, 4 ),
						new Variable( SI( 30, 5, 30, 13 ), "testarr2", VH_CREATE_VAR ),
						new Operator( SI( 30, 14, 30, 15 ), OT_ASSIGN ),
						new Array(
							SI( 30, 16, 33, 4 ),
							{
								new Expression(
									SI( 30, 18, 30, 19 ),
									new program::Value( SI( 30, 18, 30, 19 ), VALUE( type::Int, 3 ) )
								),
								new Expression(
									SI( 30, 21, 30, 27 ),
									new program::Value( SI( 30, 21, 30, 27 ), VALUE( type::String, "TEST" ) )
								),
								new Expression(
									SI( 30, 29, 33, 2 ),
									new Object(
										SI( 30, 29, 33, 2 ),
										{
											{
												"key1",
												new Expression(
													SI( 31, 9, 31, 17 ),
													new program::Value( SI( 31, 9, 31, 17 ), VALUE( type::String, "value1" ) )
												)
											},
											{
												"key2",
												new Expression(
													SI( 32, 9, 32, 17 ),
													new program::Value( SI( 32, 9, 32, 17 ), VALUE( type::String, "value2" ) )
												)
											}
										}
									)
								)
							}
						)
					)
				),
				new Statement(
					SI( 34, 1, 34, 21 ),
					new Expression(
						SI( 34, 1, 34, 21 ),
						new Variable( SI( 34, 1, 34, 9 ), "testarr1" ),
						new Operator( SI( 34, 10, 34, 13 ), OT_APPEND ),
						new program::Value( SI( 34, 14, 34, 21 ), VALUE( type::String, "first" ) )
					)
				),
				new Statement(
					SI( 35, 1, 35, 22 ),
					new Expression(
						SI( 35, 1, 35, 22 ),
						new Variable( SI( 35, 1, 35, 9 ), "testarr1" ),
						new Operator( SI( 35, 10, 35, 13 ), OT_APPEND ),
						new program::Value( SI( 35, 14, 35, 22 ), VALUE( type::String, "second" ) )
					)
				),
				new Statement(
					SI( 36, 1, 36, 23 ),
					new Expression(
						SI( 36, 1, 36, 23 ),
						new Variable( SI( 36, 1, 36, 9 ), "testarr1" ),
						new Operator( SI( 36, 10, 36, 13 ), OT_APPEND ),
						new Expression(
							SI( 36, 14, 36, 23 ),
							new Expression(
								SI( 36, 14, 36, 19 ),
								new program::Value( SI( 36, 14, 36, 15 ), VALUE( type::Int, 1 ) ),
								new Operator( SI( 36, 16, 36, 17 ), OT_ADD ),
								new program::Value( SI( 36, 18, 36, 19 ), VALUE( type::Int, 2 ) )
							),
							new Operator( SI( 36, 20, 36, 21 ), OT_ADD ),
							new program::Value( SI( 36, 22, 36, 23 ), VALUE( type::Int, 3 ) )
						)
					)
				),
				new Statement(
					SI( 37, 1, 37, 21 ),
					new Expression(
						SI( 37, 1, 37, 21 ),
						new Variable( SI( 37, 1, 37, 9 ), "testarr1" ),
						new Operator( SI( 37, 10, 37, 12 ), OT_INC_BY ),
						new Variable( SI( 37, 13, 37, 21 ), "testarr2" )
					)
				),
				new Statement(
					SI( 38, 1, 38, 22 ),
					new Expression(
						SI( 38, 1, 38, 22 ),
						new Variable( SI( 38, 1, 38, 9 ), "testarr1" ),
						new Operator( SI( 38, 10, 38, 13 ), OT_APPEND ),
						new Variable( SI( 38, 14, 38, 22 ), "testarr2" )
					)
				),
				new Statement(
					SI( 39, 1, 39, 24 ),
					new Expression(
						SI( 39, 5, 39, 24 ),
						new Variable( SI( 39, 5, 39, 13 ), "testarr3", VH_CREATE_VAR ),
						new Operator( SI( 39, 14, 39, 15 ), OT_ASSIGN ),
						new Variable( SI( 39, 16, 39, 24 ), "testarr1" )
					)
				),
				new Statement(
					SI( 40, 1, 40, 23 ),
					new Expression(
						SI( 40, 1, 40, 23 ),
						new Expression(
							SI( 40, 1, 40, 12 ),
							new Variable( SI( 40, 1, 40, 9 ), "testarr3" ),
							new Operator( SI( 40, 9, 40, 12 ), OT_AT ),
							new program::Value( SI( 40, 10, 40, 11 ), VALUE( type::Int, 1 ) )
						),
						new Operator( SI( 40, 13, 40, 14 ), OT_ASSIGN ),
						new program::Value( SI( 40, 15, 40, 23 ), VALUE( type::String, "SECOND" ) )
					)
				),
				new Statement(
					SI( 41, 1, 41, 48 ),
					new Expression(
						SI( 41, 1, 41, 48 ),
						new Expression(
							SI( 41, 1, 41, 38 ),
							new Variable( SI( 41, 1, 41, 9 ), "testarr3" ),
							new Operator( SI( 41, 9, 41, 38 ), OT_AT ),
							new Expression(
								SI( 41, 11, 41, 36 ),
								new Call(
									SI( 41, 11, 41, 31 ),
									new Expression(
										SI( 41, 11, 41, 22 ),
										new Variable( SI( 41, 11, 41, 22 ), "testmethod2" )
									),
									{
										{
											new Expression(
												SI( 41, 23, 41, 24 ),
												new Variable( SI( 41, 23, 41, 24 ), "a" )
											),
											new Expression(
												SI( 41, 26, 41, 27 ),
												new Variable( SI( 41, 26, 41, 27 ), "b" )
											),
											new Expression(
												SI( 41, 29, 41, 30 ),
												new Variable( SI( 41, 29, 41, 30 ), "c" )
											)
										}
									}
								),
								new Operator( SI( 41, 32, 41, 33 ), OT_ADD ),
								new program::Value( SI( 41, 34, 41, 36 ), VALUE( type::Int, 61 ) )
							)
						),
						new Operator( SI( 41, 39, 41, 40 ), OT_ASSIGN ),
						new program::Value( SI( 41, 41, 41, 48 ), VALUE( type::String, "FIRST" ) )
					)
				),
				new Statement(
					SI( 42, 1, 42, 46 ),
					new Expression(
						SI( 42, 1, 42, 46 ),
						new Expression(
							SI( 42, 1, 42, 14 ),
							new Variable( SI( 42, 1, 42, 9 ), "testarr3" ),
							new Operator( SI( 42, 9, 42, 14 ), OT_AT ),
							new Expression(
								SI( 42, 10, 42, 13 ),
								new program::Value( SI( 42, 10, 42, 11 ), VALUE( type::Int, 2 ) ),
								new Operator( SI( 42, 11, 42, 12 ), OT_RANGE ),
								new program::Value( SI( 42, 12, 42, 13 ), VALUE( type::Int, 5 ) )
							)
						),
						new Operator( SI( 42, 15, 42, 16 ), OT_ASSIGN ),
						new Expression(
							SI( 42, 17, 42, 46 ),
							new Expression(
								SI( 42, 17, 42, 30 ),
								new Variable( SI( 42, 17, 42, 25 ), "testarr1" ),
								new Operator( SI( 42, 25, 42, 30 ), OT_AT ),
								new Expression(
									SI( 42, 26, 42, 29 ),
									new program::Value( SI( 42, 26, 42, 27 ), VALUE( type::Int, 0 ) ),
									new Operator( SI( 42, 27, 42, 28 ), OT_RANGE ),
									new program::Value( SI( 42, 28, 42, 29 ), VALUE( type::Int, 1 ) )
								)
							),
							new Operator( SI( 42, 31, 42, 32 ), OT_ADD ),
							new Expression(
								SI( 42, 33, 42, 46 ),
								new Variable( SI( 42, 33, 42, 41 ), "testarr2" ),
								new Operator( SI( 42, 41, 42, 46 ), OT_AT ),
								new Expression(
									SI( 42, 42, 42, 45 ),
									new program::Value( SI( 42, 42, 42, 43 ), VALUE( type::Int, 0 ) ),
									new Operator( SI( 42, 43, 42, 44 ), OT_RANGE ),
									new program::Value( SI( 42, 44, 42, 45 ), VALUE( type::Int, 1 ) )
								)
							)
						)
					)
				),
				new Statement(
					SI( 43, 1, 43, 28 ),
					new Expression(
						SI( 43, 5, 43, 28 ),
						new Variable( SI( 43, 5, 43, 13 ), "testarr4", VH_CREATE_VAR ),
						new Operator( SI( 43, 14, 43, 15 ), OT_ASSIGN ),
						new Expression(
							SI( 43, 16, 43, 28 ),
							new Variable( SI( 43, 16, 43, 24 ), "testarr3" ),
							new Operator( SI( 43, 24, 43, 28 ), OT_AT ),
							new Expression(
								SI( 43, 25, 43, 27 ),
								nullptr,
								new Operator( SI( 43, 25, 43, 26 ), OT_RANGE ),
								new program::Value( SI( 43, 26, 43, 27 ), VALUE( type::Int, 3 ) )
							)
						)
					)
				),
				new Statement(
					SI( 44, 1, 44, 70 ),
					new Expression(
						SI( 44, 1, 44, 70 ),
						new Expression(
							SI( 44, 1, 44, 38 ),
							new Variable( SI( 44, 1, 44, 9 ), "testarr4" ),
							new Operator( SI( 44, 9, 44, 38 ), OT_AT ),
							new Expression(
								SI( 44, 11, 44, 36 ),
								new Expression(
									SI( 44, 11, 44, 22 ),
									new Expression(
										SI( 44, 11, 44, 16 ),
										new Variable( SI( 44, 11, 44, 12 ), "c" ),
										new Operator( SI( 44, 13, 44, 14 ), OT_ADD ),
										new program::Value( SI( 44, 15, 44, 16 ), VALUE( type::Int, 1 ) )
									),
									new Operator( SI( 44, 17, 44, 18 ), OT_SUB ),
									new program::Value( SI( 44, 19, 44, 22 ), VALUE( type::Int, 100 ) )
								),
								new Operator( SI( 44, 23, 44, 24 ), OT_RANGE ),
								new Expression(
									SI( 44, 25, 44, 36 ),
									new Expression(
										SI( 44, 25, 44, 32 ),
										new Variable( SI( 44, 25, 44, 26 ), "c" ),
										new Operator( SI( 44, 27, 44, 28 ), OT_SUB ),
										new program::Value( SI( 44, 29, 44, 32 ), VALUE( type::Int, 100 ) )
									),
									new Operator( SI( 44, 33, 44, 34 ), OT_ADD ),
									new program::Value( SI( 44, 35, 44, 36 ), VALUE( type::Int, 2 ) )
								)
							)
						),
						new Operator( SI( 44, 39, 44, 40 ), OT_ASSIGN ),
						new Array(
							SI( 44, 41, 44, 70 ),
							{
								new Expression(
									SI( 44, 43, 44, 54 ),
									new program::Value( SI( 44, 43, 44, 54 ), VALUE( type::String, "new first" ) )
								),
								new Expression(
									SI( 44, 56, 44, 68 ),
									new program::Value( SI( 44, 56, 44, 68 ), VALUE( type::String, "new second" ) )
								)
							}
						)
					)
				),
				new Statement(
					SI( 46, 1, 46, 18 ),
					new Expression(
						SI( 46, 5, 46, 18 ),
						new Variable( SI( 46, 5, 46, 13 ), "testobj1", VH_CREATE_VAR ),
						new Operator( SI( 46, 14, 46, 15 ), OT_ASSIGN ),
						new Object( SI( 46, 16, 46, 18 ), {} )
					)
				),
				new Statement(
					SI( 47, 1, 51, 2 ),
					new Expression(
						SI( 47, 5, 51, 2 ),
						new Variable( SI( 47, 5, 47, 13 ), "testobj2", VH_CREATE_VAR ),
						new Operator( SI( 47, 14, 47, 15 ), OT_ASSIGN ),
						new Object(
							SI( 47, 16, 51, 2 ),
							{
								{
									"propertyString",
									new Expression(
										SI( 48, 18, 48, 26 ),
										new program::Value( SI( 48, 18, 48, 26 ), VALUE( type::String, "STRING" ) )
									)
								},
								{
									"propertyInt1",
									new Expression(
										SI( 49, 16, 49, 27 ),
										new Expression(
											SI( 49, 16, 49, 23 ),
											new program::Value( SI( 49, 16, 49, 19 ), VALUE( type::Int, 111 ) ),
											new Operator( SI( 49, 20, 49, 21 ), OT_ADD ),
											new Variable( SI( 49, 22, 49, 23 ), "a" )
										),
										new Operator( SI( 49, 24, 49, 25 ), OT_ADD ),
										new Variable( SI( 49, 26, 49, 27 ), "b" )
									)
								},
								{
									"propertyInt2",
									new Expression(
										SI( 50, 16, 50, 19 ),
										new program::Value( SI( 50, 16, 50, 19 ), VALUE( type::Int, 222 ) )
									)
								}
							}
						)
					)
				),
				new Statement(
					SI( 52, 1, 58, 2 ),
					new Expression(
						SI( 52, 5, 58, 2 ),
						new Variable( SI( 52, 5, 52, 13 ), "testobj3", VH_CREATE_VAR ),
						new Operator( SI( 52, 14, 52, 15 ), OT_ASSIGN ),
						new Object(
							SI( 52, 16, 58, 2 ),
							{
								{
									"child1",
									new Expression(
										SI( 53, 11, 57, 4 ),
										new Object(
											SI( 53, 11, 57, 4 ),
											{
												{
													"child2",
													new Expression(
														SI( 54, 13, 56, 6 ),
														new Object(
															SI( 54, 13, 56, 6 ),
															{
																{
																	"value",
																	new Expression(
																		SI( 55, 14, 55, 27 ),
																		new program::Value( SI( 55, 14, 55, 27 ), VALUE( type::String, "CHILD VALUE" ) )
																	)
																}
															}
														)
													)
												}
											}
										)
									)
								}
							}
						)
					)
				),
				new Statement(
					SI( 59, 1, 59, 69 ),
					new Expression(
						SI( 59, 1, 59, 69 ),
						new Expression(
							SI( 59, 1, 59, 21 ),
							new Variable( SI( 59, 1, 59, 9 ), "testobj1" ),
							new Operator( SI( 59, 9, 59, 10 ), OT_CHILD ),
							new Variable( SI( 59, 10, 59, 21 ), "propertyInt" )
						),
						new Operator( SI( 59, 22, 59, 23 ), OT_ASSIGN ),
						new Expression(
							SI( 59, 24, 59, 69 ),
							new Expression(
								SI( 59, 24, 59, 45 ),
								new Variable( SI( 59, 24, 59, 32 ), "testobj2" ),
								new Operator( SI( 59, 32, 59, 33 ), OT_CHILD ),
								new Variable( SI( 59, 33, 59, 45 ), "propertyInt1" )
							),
							new Operator( SI( 59, 46, 59, 47 ), OT_ADD ),
							new Expression(
								SI( 59, 48, 59, 69 ),
								new Variable( SI( 59, 48, 59, 56 ), "testobj2" ),
								new Operator( SI( 59, 56, 59, 57 ), OT_CHILD ),
								new Variable( SI( 59, 57, 59, 69 ), "propertyInt2" )
							)
						)
					)
				),
				new Statement(
					SI( 61, 1, 61, 13 ),
					new Expression(
						SI( 61, 5, 61, 13 ),
						new Variable( SI( 61, 5, 61, 6 ), "d", VH_CREATE_VAR ),
						new Operator( SI( 61, 7, 61, 8 ), OT_ASSIGN ),
						new program::Value( SI( 61, 9, 61, 13 ), VALUE( type::Null ) )
					)
				),
				new Statement(
					SI( 62, 1, 62, 14 ),
					new Expression(
						SI( 62, 5, 62, 14 ),
						new Variable( SI( 62, 5, 62, 6 ), "x", VH_CREATE_VAR ),
						new Operator( SI( 62, 7, 62, 8 ), OT_ASSIGN ),
						new Expression(
							SI( 62, 9, 62, 14 ),
							new Variable( SI( 62, 9, 62, 10 ), "a" ),
							new Operator( SI( 62, 11, 62, 12 ), OT_GT ),
							new Variable( SI( 62, 13, 62, 14 ), "b" )
						)
					)
				),
				print(
					64, 6,
					{
						new Expression(
							SI( 64, 14, 64, 15 ),
							new Variable( SI( 64, 14, 64, 15 ), "d" )
						)
					}
				),
				print(
					65, 6,
					{
						new Expression(
							SI( 65, 14, 65, 23 ),
							new Variable( SI( 65, 14, 65, 15 ), "d" ),
							new Operator( SI( 65, 16, 65, 18 ), OT_EQ ),
							new program::Value( SI( 65, 19, 65, 23 ), VALUE( type::Null ) )
						)
					}
				),
				print(
					66, 6,
					{
						new Expression(
							SI( 66, 14, 66, 15 ),
							new Variable( SI( 66, 14, 66, 15 ), "x" )
						),
						new Expression(
							SI( 66, 17, 66, 27 ),
							new Variable( SI( 66, 17, 66, 18 ), "x" ),
							new Operator( SI( 66, 19, 66, 21 ), OT_EQ ),
							new Expression(
								SI( 66, 22, 66, 27 ),
								new Variable( SI( 66, 22, 66, 23 ), "b" ),
								new Operator( SI( 66, 24, 66, 25 ), OT_GT ),
								new Variable( SI( 66, 26, 66, 27 ), "c" )
							)
						)
					}
				),
				print(
					68, 6,
					{
						new Expression(
							SI( 68, 14, 68, 20 ),
							new Variable( SI( 68, 14, 68, 15 ), "a" ),
							new Operator( SI( 68, 16, 68, 18 ), OT_NE ),
							new Variable( SI( 68, 19, 68, 20 ), "b" )
						),
						new Expression(
							SI( 68, 22, 68, 28 ),
							new Variable( SI( 68, 22, 68, 23 ), "b" ),
							new Operator( SI( 68, 24, 68, 26 ), OT_NE ),
							new Variable( SI( 68, 27, 68, 28 ), "c" )
						),
						new Expression(
							SI( 68, 30, 68, 36 ),
							new Variable( SI( 68, 30, 68, 31 ), "c" ),
							new Operator( SI( 68, 32, 68, 34 ), OT_NE ),
							new Variable( SI( 68, 35, 68, 36 ), "a" )
						),
						new Expression(
							SI( 68, 38, 68, 44 ),
							new Variable( SI( 68, 38, 68, 39 ), "a" ),
							new Operator( SI( 68, 40, 68, 42 ), OT_NE ),
							new Variable( SI( 68, 43, 68, 44 ), "a" )
						),
					}
				),
				print(
					69, 6,
					{
						new Expression(
							SI( 69, 14, 69, 19 ),
							new Variable( SI( 69, 14, 69, 15 ), "a" ),
							new Operator( SI( 69, 16, 69, 17 ), OT_GT ),
							new Variable( SI( 69, 18, 69, 19 ), "b" )
						),
						new Expression(
							SI( 69, 21, 69, 26 ),
							new Variable( SI( 69, 21, 69, 22 ), "b" ),
							new Operator( SI( 69, 23, 69, 24 ), OT_GT ),
							new Variable( SI( 69, 25, 69, 26 ), "c" )
						)
					}
				),
				print(
					70, 6,
					{
						new Expression(
							SI( 70, 14, 70, 20 ),
							new Variable( SI( 70, 14, 70, 15 ), "b" ),
							new Operator( SI( 70, 16, 70, 18 ), OT_GTE ),
							new Variable( SI( 70, 19, 70, 20 ), "a" )
						),
						new Expression(
							SI( 70, 22, 70, 28 ),
							new Variable( SI( 70, 22, 70, 23 ), "a" ),
							new Operator( SI( 70, 24, 70, 26 ), OT_GTE ),
							new program::Value( SI( 70, 27, 70, 28 ), VALUE( type::Int, 2 ) )
						),
						new Expression(
							SI( 70, 30, 70, 38 ),
							new Variable( SI( 70, 30, 70, 31 ), "c" ),
							new Operator( SI( 70, 32, 70, 34 ), OT_LTE ),
							new program::Value( SI( 70, 35, 70, 38 ), VALUE( type::Int, 200 ) )
						),
						new Expression(
							SI( 70, 40, 70, 48 ),
							new Variable( SI( 70, 40, 70, 41 ), "a" ),
							new Operator( SI( 70, 42, 70, 44 ), OT_LTE ),
							new program::Value( SI( 70, 45, 70, 48 ), VALUE( type::Int, 200 ) )
						)
					}
				),
				print(
					71, 6,
					{
						new Expression(
							SI( 71, 14, 71, 21 ),
							new program::Value( SI( 71, 14, 71, 16 ), VALUE( type::Int, 10 ) ),
							new Operator( SI( 71, 17, 71, 18 ), OT_LT ),
							new program::Value( SI( 71, 19, 71, 21 ), VALUE( type::Int, 10 ) )
						),
						new Expression(
							SI( 71, 23, 71, 31 ),
							new program::Value( SI( 71, 23, 71, 25 ), VALUE( type::Int, 10 ) ),
							new Operator( SI( 71, 26, 71, 28 ), OT_LTE ),
							new program::Value( SI( 71, 29, 71, 31 ), VALUE( type::Int, 10 ) )
						),
						new Expression(
							SI( 71, 33, 71, 38 ),
							new Variable( SI( 71, 33, 71, 34 ), "a" ),
							new Operator( SI( 71, 35, 71, 36 ), OT_LT ),
							new Variable( SI( 71, 37, 71, 38 ), "a" )
						),
						new Expression(
							SI( 71, 40, 71, 46 ),
							new Variable( SI( 71, 40, 71, 41 ), "a" ),
							new Operator( SI( 71, 42, 71, 44 ), OT_LTE ),
							new Variable( SI( 71, 45, 71, 46 ), "a" )
						)
					}
				),
				print(
					72, 6,
					{
						new Expression(
							SI( 72, 14, 72, 26 ),
							new program::Value( SI( 72, 14, 72, 18 ), VALUE( type::Bool, true ) ),
							new Operator( SI( 72, 19, 72, 21 ), OT_AND ),
							new program::Value( SI( 72, 22, 72, 26 ), VALUE( type::Bool, true ) )
						),
						new Expression(
							SI( 72, 28, 72, 41 ),
							new program::Value( SI( 72, 28, 72, 32 ), VALUE( type::Bool, true ) ),
							new Operator( SI( 72, 33, 72, 35 ), OT_AND ),
							new program::Value( SI( 72, 36, 72, 41 ), VALUE( type::Bool, false ) )
						),
						new Expression(
							SI( 72, 43, 72, 55 ),
							new program::Value( SI( 72, 43, 72, 47 ), VALUE( type::Bool, true ) ),
							new Operator( SI( 72, 48, 72, 50 ), OT_OR ),
							new program::Value( SI( 72, 51, 72, 55 ), VALUE( type::Bool, true ) )
						),
						new Expression(
							SI( 72, 57, 72, 70 ),
							new program::Value( SI( 72, 57, 72, 61 ), VALUE( type::Bool, true ) ),
							new Operator( SI( 72, 62, 72, 64 ), OT_OR ),
							new program::Value( SI( 72, 65, 72, 70 ), VALUE( type::Bool, false ) )
						)
					}
				),
				print(
					73, 6,
					{
						new Expression(
							SI( 73, 17, 73, 85 ),
							new Expression(
								SI( 73, 17, 73, 36 ),
								new Expression(
									SI( 73, 17, 73, 23 ),
									new program::Value( SI( 73, 17, 73, 18 ), VALUE( type::Int, 5 ) ),
									new Operator( SI( 73, 19, 73, 20 ), OT_GT ),
									new program::Value( SI( 73, 21, 73, 23 ), VALUE( type::Int, 10 ) )
								),
								new Operator( SI( 73, 26, 73, 28 ), OT_AND ),
								new Expression(
									SI( 73, 31, 73, 36 ),
									new program::Value( SI( 73, 31, 73, 32 ), VALUE( type::Int, 2 ) ),
									new Operator( SI( 73, 33, 73, 34 ), OT_GT ),
									new program::Value( SI( 73, 35, 73, 36 ), VALUE( type::Int, 1 ) )
								)
							),
							new Operator( SI( 73, 40, 73, 42 ), OT_OR ),
							new Expression(
								SI( 73, 46, 73, 85 ),
								new Expression(
									SI( 73, 46, 73, 68 ),
									new Expression(
										SI( 73, 46, 73, 53 ),
										new program::Value( SI( 73, 46, 73, 47 ), VALUE( type::Int, 5 ) ),
										new Operator( SI( 73, 48, 73, 50 ), OT_LTE ),
										new program::Value( SI( 73, 51, 73, 53 ), VALUE( type::Int, 10 ) )
									),
									new Operator( SI( 73, 56, 73, 58 ), OT_AND ),
									new Expression(
										SI( 73, 59, 73, 68 ),
										nullptr,
										new Operator( SI( 73, 59, 73, 60 ), OT_NOT ),
										new Expression(
											SI( 73, 62, 73, 68 ),
											new program::Value( SI( 73, 62, 73, 63 ), VALUE( type::Int, 5 ) ),
											new Operator( SI( 73, 64, 73, 65 ), OT_GT ),
											new program::Value( SI( 73, 66, 73, 68 ), VALUE( type::Int, 35 ) )
										)
									)
								),
								new Operator( SI( 73, 71, 73, 73 ), OT_AND ),
								new Expression(
									SI( 73, 76, 73, 85 ),
									new program::Value( SI( 73, 76, 73, 79 ), VALUE( type::Int, 100 ) ),
									new Operator( SI( 73, 80, 73, 82 ), OT_GTE ),
									new program::Value( SI( 73, 83, 73, 85 ), VALUE( type::Int, 20 ) )
								)
							)
						)
					}
				),
				print(
					74, 6,
					{
						new Expression(
							SI( 74, 13, 74, 35 ),
							new Call(
								SI( 74, 13, 74, 35 ),
								new Expression(
									SI( 74, 13, 74, 24 ),
									new Variable( SI( 74, 13, 74, 24 ), "testmethod1" )
								),
								{
									new Expression(
										SI( 74, 25, 74, 27 ),
										new program::Value( SI( 74, 25, 74, 27 ), VALUE( type::Int, 11 ) )
									),
									new Expression(
										SI( 74, 29, 74, 30 ),
										new Variable( SI( 74, 29, 74, 30 ), "b" )
									),
									new Expression(
										SI( 74, 32, 74, 34 ),
										new program::Value( SI( 74, 32, 74, 34 ), VALUE( type::Int, 20 ) )
									)
								}
							)
						),
						new Expression(
							SI( 74, 37, 74, 57 ),
							new Call(
								SI( 74, 37, 74, 57 ),
								new Expression(
									SI( 74, 37, 74, 48 ),
									new Variable( SI( 74, 37, 74, 48 ), "testmethod2" )
								),
								{
									new Expression(
										SI( 74, 49, 74, 50 ),
										new Variable( SI( 74, 49, 74, 50 ), "a" )
									),
									new Expression(
										SI( 74, 52, 74, 53 ),
										new Variable( SI( 74, 52, 74, 53 ), "b" )
									),
									new Expression(
										SI( 74, 55, 74, 56 ),
										new Variable( SI( 74, 55, 74, 56 ), "c" )
									)
								}
							)
						)
					}
				),
				new Statement(
					SI( 75, 1, 75, 29 ),
					new Expression(
						SI( 75, 5, 75, 29 ),
						new Variable( SI( 75, 5, 75, 15 ), "testmethod", VH_CREATE_VAR ),
						new Operator( SI( 75, 16, 75, 17 ), OT_ASSIGN ),
						new Variable( SI( 75, 18, 75, 29 ), "testmethod1" )
					)
				),
				print(
					76, 6,
					{
						new Expression(
							SI( 76, 14, 76, 134 ),
							new Call(
								SI( 76, 14, 76, 134 ),
								new Expression(
									SI( 76, 14, 76, 24 ),
									new Variable( SI( 76, 14, 76, 24 ), "testmethod" )
								),
								{
									new Expression(
										SI( 76, 26, 76, 27 ),
										new program::Value( SI( 76, 26, 76, 27 ), VALUE( type::Int, 1 ) )
									),
									new Expression(
										SI( 76, 29, 76, 90 ),
										new Call(
											SI( 76, 29, 76, 90 ),
											new Expression(
												SI( 76, 29, 76, 39 ),
												new Variable( SI( 76, 29, 76, 39 ), "testmethod" )
											),
											{
												new Expression(
													SI( 76, 41, 76, 42 ),
													new program::Value( SI( 76, 41, 76, 42 ), VALUE( type::Int, 2 ) )
												),
												new Expression(
													SI( 76, 44, 76, 65 ),
													new Call(
														SI( 76, 44, 76, 65 ),
														new Expression(
															SI( 76, 44, 76, 54 ),
															new Variable( SI( 76, 44, 76, 54 ), "testmethod" )
														),
														{
															new Expression(
																SI( 76, 56, 76, 57 ),
																new program::Value( SI( 76, 56, 76, 57 ), VALUE( type::Int, 3 ) )
															),
															new Expression(
																SI( 76, 59, 76, 60 ),
																new program::Value( SI( 76, 59, 76, 60 ), VALUE( type::Int, 3 ) )
															),
															new Expression(
																SI( 76, 62, 76, 63 ),
																new program::Value( SI( 76, 62, 76, 63 ), VALUE( type::Int, 3 ) )
															)
														}
													)
												),
												new Expression(
													SI( 76, 67, 76, 88 ),
													new Call(
														SI( 76, 67, 76, 88 ),
														new Expression(
															SI( 76, 67, 76, 77 ),
															new Variable( SI( 76, 67, 76, 77 ), "testmethod" )
														),
														{
															new Expression(
																SI( 76, 79, 76, 80 ),
																new program::Value( SI( 76, 79, 76, 80 ), VALUE( type::Int, 4 ) )
															),
															new Expression(
																SI( 76, 82, 76, 83 ),
																new program::Value( SI( 76, 82, 76, 83 ), VALUE( type::Int, 4 ) )
															),
															new Expression(
																SI( 76, 85, 76, 86 ),
																new program::Value( SI( 76, 85, 76, 86 ), VALUE( type::Int, 4 ) )
															)
														}
													)
												)
											}
										)
									),
									new Expression(
										SI( 76, 92, 76, 132 ),
										new Call(
											SI( 76, 92, 76, 132 ),
											new Expression(
												SI( 76, 92, 76, 102 ),
												new Variable( SI( 76, 92, 76, 102 ), "testmethod" )
											),
											{
												new Expression(
													SI( 76, 104, 76, 105 ),
													new program::Value( SI( 76, 104, 76, 105 ), VALUE( type::Int, 5 ) )
												),
												new Expression(
													SI( 76, 107, 76, 108 ),
													new program::Value( SI( 76, 107, 76, 108 ), VALUE( type::Int, 5 ) )
												),
												new Expression(
													SI( 76, 110, 76, 131 ),
													new Call(
														SI( 76, 110, 76, 131 ),
														new Expression(
															SI( 76, 110, 76, 120 ),
															new Variable( SI( 76, 110, 76, 120 ), "testmethod" )
														),
														{
															new Expression(
																SI( 76, 122, 76, 123 ),
																new program::Value( SI( 76, 122, 76, 123 ), VALUE( type::Int, 6 ) )
															),
															new Expression(
																SI( 76, 125, 76, 126 ),
																new program::Value( SI( 76, 125, 76, 126 ), VALUE( type::Int, 6 ) )
															),
															new Expression(
																SI( 76, 128, 76, 129 ),
																new program::Value( SI( 76, 128, 76, 129 ), VALUE( type::Int, 6 ) )
															)
														}
													)
												)
											}
										)
									)
								}
							)
						),
						new Expression(
							SI( 76, 136, 76, 138 ),
							new program::Value( SI( 76, 136, 76, 138 ), VALUE( type::Int, 10 ) )
						)
					}
				),
				print(
					77, 6,
					{
						new Expression(
							SI( 77, 14, 77, 22 ),
							new Variable( SI( 77, 14, 77, 22 ), "testarr1" )
						)
					}
				),
				print(
					77, 31,
					{
						new Expression(
							SI( 77, 39, 77, 47 ),
							new Variable( SI( 77, 39, 77, 47 ), "testarr2" )
						)
					}
				),
				print(
					77, 56,
					{
						new Expression(
							SI( 77, 64, 77, 72 ),
							new Variable( SI( 77, 64, 77, 72 ), "testarr3" )
						)
					}
				),
				print(
					77, 81,
					{
						new Expression(
							SI( 77, 89, 77, 97 ),
							new Variable( SI( 77, 89, 77, 97 ), "testarr4" )
						)
					}
				),
				print(
					78, 6,
					{
						new Expression(
							SI( 78, 14, 78, 25 ),
							new Variable( SI( 78, 14, 78, 22 ), "testarr1" ),
							new Operator( SI( 78, 22, 78, 25 ), OT_AT ),
							new program::Value( SI( 78, 23, 78, 24 ), VALUE( type::Int, 0 ) )
						)
					}
				),
				print(
					78, 34,
					{
						new Expression(
							SI( 78, 42, 78, 53 ),
							new Variable( SI( 78, 42, 78, 50 ), "testarr1" ),
							new Operator( SI( 78, 50, 78, 53 ), OT_AT ),
							new program::Value( SI( 78, 51, 78, 52 ), VALUE( type::Int, 1 ) )
						)
					}
				),
				print(
					78, 62,
					{
						new Expression(
							SI( 78, 70, 78, 83 ),
							new Variable( SI( 78, 70, 78, 78 ), "testarr1" ),
							new Operator( SI( 78, 78, 78, 83 ), OT_AT ),
							new Expression(
								SI( 78, 79, 78, 82 ),
								new program::Value( SI( 78, 79, 78, 80 ), VALUE( type::Int, 0 ) ),
								new Operator( SI( 78, 80, 78, 81 ), OT_RANGE ),
								new program::Value( SI( 78, 81, 78, 82 ), VALUE( type::Int, 1 ) )
							)
						)
					}
				),
				print(
					79, 6,
					{
						new Expression(
							SI( 79, 14, 79, 26 ),
							new Variable( SI( 79, 14, 79, 22 ), "testarr1" ),
							new Operator( SI( 79, 22, 79, 26 ), OT_AT ),
							new Expression(
								SI( 79, 23, 79, 25 ),
								new program::Value( SI( 79, 23, 79, 24 ), VALUE( type::Int, 5 ) ),
								new Operator( SI( 79, 24, 79, 25 ), OT_RANGE ),
								nullptr
							)
						)
					}
				),
				print(
					79, 35,
					{
						new Expression(
							SI( 79, 43, 79, 55 ),
							new Variable( SI( 79, 43, 79, 51 ), "testarr1" ),
							new Operator( SI( 79, 51, 79, 55 ), OT_AT ),
							new Expression(
								SI( 79, 52, 79, 54 ),
								nullptr,
								new Operator( SI( 79, 52, 79, 53 ), OT_RANGE ),
								new program::Value( SI( 79, 53, 79, 54 ), VALUE( type::Int, 3 ) )
							)
						)
					}
				),
				print(
					80, 6,
					{
						new Expression(
							SI( 80, 14, 80, 43 ),
							new Expression(
								SI( 80, 14, 80, 27 ),
								new Variable( SI( 80, 14, 80, 22 ), "testarr1" ),
								new Operator( SI( 80, 22, 80, 27 ), OT_AT ),
								new Expression(
									SI( 80, 23, 80, 26 ),
									new program::Value( SI( 80, 23, 80, 24 ), VALUE( type::Int, 4 ) ),
									new Operator( SI( 80, 24, 80, 25 ), OT_RANGE ),
									new program::Value( SI( 80, 25, 80, 26 ), VALUE( type::Int, 5 ) )
								)
							),
							new Operator( SI( 80, 28, 80, 29 ), OT_ADD ),
							new Expression(
								SI( 80, 30, 80, 43 ),
								new Variable( SI( 80, 30, 80, 38 ), "testarr1" ),
								new Operator( SI( 80, 38, 80, 43 ), OT_AT ),
								new Expression(
									SI( 80, 39, 80, 42 ),
									new program::Value( SI( 80, 39, 80, 40 ), VALUE( type::Int, 2 ) ),
									new Operator( SI( 80, 40, 80, 41 ), OT_RANGE ),
									new program::Value( SI( 80, 41, 80, 42 ), VALUE( type::Int, 3 ) )
								)
							)
						)
					}
				),
				print(
					81, 6,
					{
						new Expression(
							SI( 81, 13, 81, 41 ),
							new Expression(
								SI( 81, 13, 81, 35 ),
								new Expression(
									SI( 81, 13, 81, 28 ),
									new Variable( SI( 81, 13, 81, 21 ), "testobj3" ),
									new Operator( SI( 81, 21, 81, 22 ), OT_CHILD ),
									new Variable( SI( 81, 22, 81, 28 ), "child1" )
								),
								new Operator( SI( 81, 28, 81, 29 ), OT_CHILD ),
								new Variable( SI( 81, 29, 81, 35 ), "child2" )
							),
							new Operator( SI( 81, 35, 81, 36 ), OT_CHILD ),
							new Variable( SI( 81, 36, 81, 41 ), "value" )
						)
					}
				),
				print(
					82, 6,
					{
						new Expression(
							SI( 82, 13, 82, 44 ),
							new Expression(
								SI( 82, 13, 82, 33 ),
								new Variable( SI( 82, 13, 82, 21 ), "testobj1" ),
								new Operator( SI( 82, 21, 82, 22 ), OT_CHILD ),
								new Variable( SI( 82, 22, 82, 33 ), "propertyInt" )
							),
							new Operator( SI( 82, 34, 82, 36 ), OT_EQ ),
							new Expression(
								SI( 82, 37, 82, 44 ),
								new program::Value( SI( 82, 37, 82, 40 ), VALUE( type::Int, 272 ) ),
								new Operator( SI( 82, 41, 82, 42 ), OT_ADD ),
								new Variable( SI( 82, 43, 82, 44 ), "c" )
							)
						)
					}
				),
				print(
					82, 52,
					{
						new Expression(
							SI( 82, 59, 82, 67 ),
							new Variable( SI( 82, 59, 82, 67 ), "testobj1" )
						),
						new Expression(
							SI( 82, 69, 82, 77 ),
							new Variable( SI( 82, 69, 82, 77 ), "testobj2" )
						),
					}
				),
				new If(
					SI( 84, 1, 86, 2 ),
					new Expression(
						SI( 84, 6, 84, 11 ),
						new Variable( SI( 84, 6, 84, 7 ), "a" ),
						new Operator( SI( 84, 8, 84, 9 ), OT_GT ),
						new Variable( SI( 84, 10, 84, 11 ), "b" )
					),
					new Scope(
						SI( 85, 8, 85, 24 ),
						{
							print(
								85, 8,
								{
									new Expression(
										SI( 85, 16, 85, 21 ),
										new program::Value( SI( 85, 16, 85, 21 ), VALUE( type::String, "YES" ) )
									)
								}
							)
						}
					),
					new Else(
						SI( 87, 1, 89, 2 ),
						new Scope(
							SI( 88, 3, 88, 23 ),
							{
								print(
									88, 8,
									{
										new Expression(
											SI( 88, 16, 88, 20 ),
											new program::Value( SI( 88, 16, 88, 20 ), VALUE( type::String, "NO" ) )
										)
									}
								)
							}
						)
					)
				),
				new If(
					SI( 90, 1, 92, 2 ),
					new Expression(
						SI( 90, 6, 90, 11 ),
						new Variable( SI( 90, 6, 90, 7 ), "b" ),
						new Operator( SI( 90, 8, 90, 9 ), OT_GT ),
						new Variable( SI( 90, 10, 90, 11 ), "a" )
					),
					new Scope(
						SI( 91, 8, 91, 24 ),
						{
							print(
								91, 8,
								{
									new Expression(
										SI( 91, 16, 91, 21 ),
										new program::Value( SI( 91, 16, 91, 21 ), VALUE( type::String, "YES" ) )
									)
								}
							)
						}
					),
					new Else(
						SI( 93, 1, 95, 2 ),
						new Scope(
							SI( 94, 3, 94, 23 ),
							{
								print(
									94, 8,
									{
										new Expression(
											SI( 94, 16, 94, 20 ),
											new program::Value( SI( 94, 16, 94, 20 ), VALUE( type::String, "NO" ) )
										)
									}
								)
							}
						)
					)
				),
				new If(
					SI( 96, 1, 96, 41 ),
					new Expression(
						SI( 96, 6, 96, 11 ),
						new program::Value( SI( 96, 6, 96, 11 ), VALUE( type::Bool, false ) )
					),
					new Scope(
						SI( 96, 21, 96, 39 ),
						{
							print(
								96, 21,
								{
									new Expression(
										SI( 96, 29, 96, 36 ),
										new program::Value( SI( 96, 29, 96, 36 ), VALUE( type::String, "FALSE" ) )
									)
								}
							)
						}
					)
				),
				new If(
					SI( 97, 1, 99, 2 ),
					new Expression(
						SI( 97, 6, 97, 11 ),
						new program::Value( SI( 97, 6, 97, 11 ), VALUE( type::Bool, false ) )
					),
					new Scope(
						SI( 98, 8, 98, 23 ),
						{
							print(
								98, 8,
								{
									new Expression(
										SI( 98, 15, 98, 21 ),
										new program::Value( SI( 98, 15, 98, 21 ), VALUE( type::String, "FAIL" ) )
									)
								}
							)
						}
					),
					new ElseIf(
						SI( 99, 3, 101, 2 ),
						new Expression(
							SI( 99, 12, 99, 17 ),
							new program::Value( SI( 99, 12, 99, 17 ), VALUE( type::Bool, false ) )
						),
						new Scope(
							SI( 100, 3, 100, 25 ),
							{
								print(
									100, 8,
									{
										new Expression(
											SI( 100, 16, 100, 22 ),
											new program::Value( SI( 100, 16, 100, 22 ), VALUE( type::String, "FAIL" ) )
										)
									}
								)
							}
						),
						new ElseIf(
							SI( 101, 3, 103, 2 ),
							new Expression(
								SI( 101, 12, 101, 16 ),
								new program::Value( SI( 101, 12, 101, 16 ), VALUE( type::Bool, true ) )
							),
							new Scope(
								SI( 102, 3, 102, 23 ),
								{
									print(
										102, 8,
										{
											new Expression(
												SI( 102, 16, 102, 20 ),
												new program::Value( SI( 102, 16, 102, 20 ), VALUE( type::String, "OK" ) )
											)
										}
									)
								}
							),
							new Else(
								SI( 103, 3, 105, 2 ),
								new Scope(
									SI( 104, 3, 104, 25 ),
									{
										print(
											104, 8,
											{
												new Expression(
													SI( 104, 16, 104, 22 ),
													new program::Value( SI( 104, 16, 104, 22 ), VALUE( type::String, "FAIL" ) )
												)
											}
										)
									}
								)
							)
						)
					)
				),
				new Statement(
					SI( 107, 1, 107, 10 ),
					new Expression(
						SI( 107, 5, 107, 10 ),
						new Variable( SI( 107, 5, 107, 6 ), "i", VH_CREATE_VAR ),
						new Operator( SI( 107, 7, 107, 8 ), OT_ASSIGN ),
						new program::Value( SI( 107, 9, 107, 10 ), VALUE( type::Int, 0 ) )
					)
				),
				new While(
					SI( 108, 1, 110, 2 ),
					new Expression(
						SI( 108, 9, 108, 16 ),
						new Expression(
							SI( 108, 9, 108, 12 ),
							new Variable( SI( 108, 9, 108, 10 ), "i" ),
							new Operator( SI( 108, 10, 108, 12 ), OT_INC )
						),
						new Operator( SI( 108, 13, 108, 14 ), OT_LT ),
						new program::Value( SI( 108, 15, 108, 16 ), VALUE( type::Int, 5 ) )
					),
					new Scope(
						SI( 109, 8, 109, 18 ),
						{
							print(
								109, 8,
								{
									new Expression(
										SI( 109, 15, 109, 16 ),
										new Variable( SI( 109, 15, 109, 16 ), "i" )
									)
								}
							)
						}
					)
				),
				new Try(
					SI( 112, 1, 120, 2 ),
					new Scope(
						SI( 113, 8, 119, 36 ),
						{
							print(
								113, 8,
								{
									new Expression(
										SI( 113, 16, 113, 34 ),
										new program::Value( SI( 113, 16, 113, 34 ), VALUE( type::String, "BEFORE EXCEPTION" ) )
									)
								}
							),
							new Statement(
								SI( 114, 3, 117, 4 ),
								new Expression(
									SI( 114, 7, 117, 4 ),
									new Variable( SI( 114, 7, 114, 15 ), "failfunc", VH_CREATE_VAR ),
									new Operator( SI( 114, 16, 114, 17 ), OT_ASSIGN ),
									new Function(
										SI( 114, 18, 117, 4 ),
										{},
										new Scope(
											SI( 115, 10, 116, 43 ),
											{
												print(
													115, 10,
													{
														new Expression(
															SI( 115, 17, 115, 27 ),
															new program::Value( SI( 115, 17, 115, 27 ), VALUE( type::String, "failfunc" ) )
														),
													}
												),
												new Statement(
													SI( 116, 5, 116, 42 ),
													new Expression(
														SI( 116, 5, 116, 42 ),
														nullptr,
														new Operator( SI( 116, 5, 116, 10 ), OT_THROW ),
														new Call(
															SI( 116, 11, 116, 42 ),
															new Expression(
																SI( 116, 11, 116, 20 ),
																new Variable( SI( 116, 11, 116, 20 ), "TestError" )
															),
															{
																new Expression(
																	SI( 116, 21, 116, 41 ),
																	new program::Value( SI( 116, 21, 116, 41 ), VALUE( type::String, "something happened" ) )
																)
															}
														)
													)
												),
											}
										)
									)
								)
							),
							new Statement(
								SI( 118, 3, 118, 13 ),
								new Expression(
									SI( 118, 3, 118, 13 ),
									new Call(
										SI( 118, 3, 118, 13 ),
										new Expression(
											SI( 118, 3, 118, 11 ),
											new Variable( SI( 118, 3, 118, 11 ), "failfunc" )
										),
										{}
									)
								)
							),
							print(
								119, 8,
								{
									new Expression(
										SI( 119, 16, 119, 33 ),
										new program::Value( SI( 119, 16, 119, 33 ), VALUE( type::String, "AFTER EXCEPTION" ) )
									)
								}
							),
						}
					),
					new Catch(
						SI( 121, 1, 129, 2 ),
						new Object(
							SI( 121, 7, 129, 2 ),
							{
								{
									"UnknownError",
									new Expression(
										SI( 122, 17, 124, 4 ),
										new Function(
											SI( 122, 17, 124, 4 ),
											{
												new Variable( SI( 122, 18, 122, 19 ), "e" )
											}, new Scope(
												SI( 123, 10, 123, 40 ),
												{
													print(
														123, 10,
														{
															new Expression(
																SI( 123, 17, 123, 38 ),
																new program::Value( SI( 123, 17, 123, 38 ), VALUE( type::String, "shouldnt catch this" ) )
															)
														}
													)
												}
											)
										)
									)
								},
								{
									"TestError",
									new Expression(
										SI( 125, 14, 128, 4 ),
										new Function(
											SI( 125, 14, 128, 4 ),
											{
												new Variable( SI( 125, 15, 125, 16 ), "e" )
											}, new Scope(
												SI( 126, 10, 127, 30 ),
												{
													print(
														126, 10,
														{
															new Expression(
																SI( 126, 17, 126, 54 ),
																new Expression(
																	SI( 126, 17, 126, 43 ),
																	new Expression(
																		SI( 126, 17, 126, 35 ),
																		new program::Value( SI( 126, 17, 126, 26 ), VALUE( type::String, "CAUGHT " ) ),
																		new Operator( SI( 126, 27, 126, 28 ), OT_ADD ),
																		new Expression(
																			SI( 126, 29, 126, 35 ),
																			new Variable( SI( 126, 29, 126, 30 ), "e" ),
																			new Operator( SI( 126, 30, 126, 31 ), OT_CHILD ),
																			new Variable( SI( 126, 31, 126, 35 ), "type" )
																		)
																	),
																	new Operator( SI( 126, 36, 126, 37 ), OT_ADD ),
																	new program::Value( SI( 126, 38, 126, 43 ), VALUE( type::String, " : " ) )
																),
																new Operator( SI( 126, 44, 126, 45 ), OT_ADD ),
																new Expression(
																	SI( 126, 46, 126, 54 ),
																	new Variable( SI( 126, 46, 126, 47 ), "e" ),
																	new Operator( SI( 126, 47, 126, 48 ), OT_CHILD ),
																	new Variable( SI( 126, 48, 126, 54 ), "reason" )
																)
															)
														}
													),
													print(
														127, 10,
														{
															new Expression(
																SI( 127, 17, 127, 28 ),
																new Variable( SI( 127, 17, 127, 18 ), "e" ),
																new Operator( SI( 127, 18, 127, 19 ), OT_CHILD ),
																new Variable( SI( 127, 19, 127, 28 ), "backtrace" )
															)
														}
													)
												}
											)
										)
									)
								}
							}
						)
					)
				),
				print(
					132, 6,
					{
						new Expression(
							SI( 132, 13, 132, 19 ),
							new program::Value( SI( 132, 13, 132, 19 ), VALUE( type::String, "bye!" ) )
						)
					}
				),
			}
		)
	);
	return program;
}

const std::string& GetExpectedResult() {
	const static std::string result = "null\n"
									  "true\n"
									  "true false\n"
									  "true true true false\n"
									  "true false\n"
									  "false true true true\n"
									  "false true false true\n"
									  "true false true true\n"
									  "true\n"
									  "45 -61\n"
									  "52 10\n"
									  "[ first, second, 6, 3, TEST, { key1: value1, key2: value2 }, [ 3, TEST, { key1: value1, key2: value2 } ] ]\n"
									  "[ 3, TEST, { key1: value1, key2: value2 } ]\n"
									  "[ FIRST, SECOND, first, second, 3, TEST, [ 3, TEST, { key1: value1, key2: value2 } ] ]\n"
									  "[ FIRST, new first, new second, second ]\n"
									  "first\n"
									  "second\n"
									  "[ first, second ]\n"
									  "[ { key1: value1, key2: value2 }, [ 3, TEST, { key1: value1, key2: value2 } ] ]\n"
									  "[ first, second, 6, 3 ]\n"
									  "[ TEST, { key1: value1, key2: value2 }, 6, 3 ]\n"
									  "CHILD VALUE\n"
									  "true\n"
									  "{ propertyInt: 372 } { propertyInt1: 150, propertyInt2: 222, propertyString: STRING }\n"
									  "YES\n"
									  "NO\n"
									  "OK\n"
									  "1\n"
									  "2\n"
									  "3\n"
									  "4\n"
									  "5\n"
									  "BEFORE EXCEPTION\n"
									  "failfunc\n"
									  "CAUGHT TestError : something happened\n"
									  "[ \tat <TEST>.gls.js:116: throw TestError('something happened');, \tat <TEST>.gls.js:118: failfunc(); ]\n"
									  "bye!\n";
	return result;
}

}
}
