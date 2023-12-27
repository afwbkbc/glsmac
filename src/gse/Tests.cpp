#include "Tests.h"

#include "tests/GSE.h"
#include "tests/Parser.h"
#include "tests/Runner.h"

#include "gse/program/Variable.h"
#include "gse/program/Value.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"
#include "gse/program/Array.h"
#include "gse/type/Bool.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/type/Null.h"

namespace gse {

using namespace program;

void AddTests( task::gsetests::GSETests* task ) {

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

const Program* GetTestProgram() {

	const auto console_log = []( const std::vector< const Expression* >& arguments ) -> Statement* {
		return new Statement(
			new Expression(
				new Call(
					new Expression(
						new Variable( "console" ),
						new Operator( Operator::OT_CHILD ),
						new Variable( "log" )
					),
					arguments
				)
			)
		);
	};

	static const Program test_program(
		new Scope(
			{
				new Statement(
					new Expression(
						new Variable( "a", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new program::Value( VALUE( type::Int, 5 ) )
					)
				),
				new Statement(
					new Expression(
						new Variable( "a" ),
						new Operator( Operator::OT_INC )
					)
				),
				new Statement(
					new Expression(
						new Variable( "b", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new Expression(
							new Variable( "a" ),
							new Operator( Operator::OT_ADD ),
							new Expression(
								new program::Value( VALUE( type::Int, 2 ) ),
								new Operator( Operator::OT_MULT ),
								new program::Value( VALUE( type::Int, 4 ) )
							)
						)
					)
				),
				new Statement(
					new Expression(
						new Variable( "c", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new Expression(
							new Expression(
								new Variable( "a" ),
								new Operator( Operator::OT_ADD ),
								new program::Value( VALUE( type::Int, 2 ) )
							),
							new Operator( Operator::OT_MULT ),
							new program::Value( VALUE( type::Int, 4 ) )
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
										new Operator( Operator::OT_INC_BY ),
										new program::Value( VALUE( type::Int, 10 ) )
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
						new Variable( "c" ),
						new Operator( Operator::OT_DEC_BY ),
						new program::Value( VALUE( type::Int, 23 ) )
					)
				),
				new Statement(
					new Expression(
						new Variable( "testmethod1", Variable::VH_CREATE ),
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
											nullptr,
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
						new Variable( "testmethod2", Variable::VH_CREATE ),
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
											nullptr,
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
						new Variable( "testarr1", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new Array( {} )
					)
				),
				new Statement(
					new Expression(
						new Variable( "testarr2", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new Array(
							{
								new Expression(
									new program::Value( VALUE( type::Int, 3 ) )
								),
								new Expression(
									new program::Value( VALUE( type::String, "TEST" ) )
								),
								new Expression(
									new Object(
										{
											{
												"key1",
												new Expression(
													new program::Value( VALUE( type::String, "value1" ) )
												)
											},
											{
												"key2",
												new Expression(
													new program::Value( VALUE( type::String, "value2" ) )
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
					new Expression(
						new Variable( "testarr1" ),
						new Operator( Operator::OT_APPEND ),
						new program::Value( VALUE( type::String, "first" ) )
					)
				),
				new Statement(
					new Expression(
						new Variable( "testarr1" ),
						new Operator( Operator::OT_APPEND ),
						new program::Value( VALUE( type::String, "second" ) )
					)
				),
				new Statement(
					new Expression(
						new Variable( "testarr1" ),
						new Operator( Operator::OT_APPEND ),
						new Expression(
							new Expression(
								new program::Value( VALUE( type::Int, 1 ) ),
								new Operator( Operator::OT_ADD ),
								new program::Value( VALUE( type::Int, 2 ) )
							),
							new Operator( Operator::OT_ADD ),
							new program::Value( VALUE( type::Int, 3 ) )
						)
					)
				),
				new Statement(
					new Expression(
						new Variable( "testarr1" ),
						new Operator( Operator::OT_INC_BY ),
						new Variable( "testarr2" )
					)
				),
				new Statement(
					new Expression(
						new Variable( "testarr1" ),
						new Operator( Operator::OT_APPEND ),
						new Variable( "testarr2" )
					)
				),
				new Statement(
					new Expression(
						new Variable( "testobj1", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new Object( {} )
					)
				),
				new Statement(
					new Expression(
						new Variable( "testobj2", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new Object(
							{
								{
									"propertyString",
									new Expression(
										new program::Value( VALUE( type::String, "STRING" ) )
									)
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
									new Expression(
										new program::Value( VALUE( type::Int, 222 ) )
									)
								}
							}
						)
					)
				),
				new Statement(
					new Expression(
						new Variable( "testobj3", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new Object(
							{
								{
									"child1",
									new Expression(
										new Object(
											{
												{
													"child2",
													new Expression(
														new Object(
															{
																{
																	"value",
																	new Expression(
																		new program::Value( VALUE( type::String, "CHILD VALUE" ) )
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
						new Variable( "d", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new program::Value( VALUE( type::Null ) )
					)
				),
				new Statement(
					new Expression(
						new Variable( "x", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new Expression(
							new Variable( "a" ),
							new Operator( Operator::OT_GT ),
							new Variable( "b" )
						)
					)
				),
				console_log(
					{
						new Expression(
							new Variable( "d" )
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "d" ),
							new Operator( Operator::OT_EQ ),
							new program::Value( VALUE( type::Null ) )
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "x" )
						),
						new Expression(
							new Variable( "x" ),
							new Operator( Operator::OT_EQ ),
							new Expression(
								new Variable( "b" ),
								new Operator( Operator::OT_GT ),
								new Variable( "c" )
							)
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "a" ),
							new Operator( Operator::OT_NE ),
							new Variable( "b" )
						),
						new Expression(
							new Variable( "b" ),
							new Operator( Operator::OT_NE ),
							new Variable( "c" )
						),
						new Expression(
							new Variable( "c" ),
							new Operator( Operator::OT_NE ),
							new Variable( "a" )
						),
						new Expression(
							new Variable( "a" ),
							new Operator( Operator::OT_NE ),
							new Variable( "a" )
						),
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "a" ),
							new Operator( Operator::OT_GT ),
							new Variable( "b" )
						),
						new Expression(
							new Variable( "b" ),
							new Operator( Operator::OT_GT ),
							new Variable( "c" )
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "b" ),
							new Operator( Operator::OT_GTE ),
							new Variable( "a" )
						),
						new Expression(
							new Variable( "a" ),
							new Operator( Operator::OT_GTE ),
							new program::Value( VALUE( type::Int, 2 ) )
						),
						new Expression(
							new Variable( "c" ),
							new Operator( Operator::OT_LTE ),
							new program::Value( VALUE( type::Int, 200 ) )
						),
						new Expression(
							new Variable( "a" ),
							new Operator( Operator::OT_LTE ),
							new program::Value( VALUE( type::Int, 200 ) )
						)
					}
				),
				console_log(
					{
						new Expression(
							new program::Value( VALUE( type::Int, 10 ) ),
							new Operator( Operator::OT_LT ),
							new program::Value( VALUE( type::Int, 10 ) )
						),
						new Expression(
							new program::Value( VALUE( type::Int, 10 ) ),
							new Operator( Operator::OT_LTE ),
							new program::Value( VALUE( type::Int, 10 ) )
						),
						new Expression(
							new Variable( "a" ),
							new Operator( Operator::OT_LT ),
							new Variable( "a" )
						),
						new Expression(
							new Variable( "a" ),
							new Operator( Operator::OT_LTE ),
							new Variable( "a" )
						)
					}
				),
				console_log(
					{
						new Expression(
							new program::Value( VALUE( type::Bool, true ) ),
							new Operator( Operator::OT_AND ),
							new program::Value( VALUE( type::Bool, true ) )
						),
						new Expression(
							new program::Value( VALUE( type::Bool, true ) ),
							new Operator( Operator::OT_AND ),
							new program::Value( VALUE( type::Bool, false ) )
						),
						new Expression(
							new program::Value( VALUE( type::Bool, true ) ),
							new Operator( Operator::OT_OR ),
							new program::Value( VALUE( type::Bool, true ) )
						),
						new Expression(
							new program::Value( VALUE( type::Bool, true ) ),
							new Operator( Operator::OT_OR ),
							new program::Value( VALUE( type::Bool, false ) )
						)
					}
				),
				console_log(
					{
						new Expression(
							new Expression(
								new Expression(
									new program::Value( VALUE( type::Int, 5 ) ),
									new Operator( Operator::OT_GT ),
									new program::Value( VALUE( type::Int, 10 ) )
								),
								new Operator( Operator::OT_AND ),
								new Expression(
									new program::Value( VALUE( type::Int, 2 ) ),
									new Operator( Operator::OT_GT ),
									new program::Value( VALUE( type::Int, 1 ) )
								)
							),
							new Operator( Operator::OT_OR ),
							new Expression(
								new Expression(
									new Expression(
										new program::Value( VALUE( type::Int, 5 ) ),
										new Operator( Operator::OT_LTE ),
										new program::Value( VALUE( type::Int, 10 ) )
									),
									new Operator( Operator::OT_AND ),
									new Expression(
										nullptr,
										new Operator( Operator::OT_NOT ),
										new Expression(
											new program::Value( VALUE( type::Int, 5 ) ),
											new Operator( Operator::OT_GT ),
											new program::Value( VALUE( type::Int, 35 ) )
										)
									)
								),
								new Operator( Operator::OT_AND ),
								new Expression(
									new program::Value( VALUE( type::Int, 100 ) ),
									new Operator( Operator::OT_GTE ),
									new program::Value( VALUE( type::Int, 20 ) )
								)
							)
						)
					}
				),
				console_log(
					{
						new Expression(
							new Call(
								new Expression(
									new Variable( "testmethod1" )
								),
								{
									new Expression(
										new program::Value( VALUE( type::Int, 11 ) )
									),
									new Expression(
										new Variable( "b" )
									),
									new Expression(
										new program::Value( VALUE( type::Int, 20 ) )
									)
								}
							)
						),
						new Expression(
							new Call(
								new Expression(
									new Variable( "testmethod2" )
								),
								{
									new Expression(
										new Variable( "a" )
									),
									new Expression(
										new Variable( "b" )
									),
									new Expression(
										new Variable( "c" )
									)
								}
							)
						)
					}
				),
				new Statement(
					new Expression(
						new Variable( "testmethod", Variable::VH_CREATE ),
						new Operator( Operator::OT_ASSIGN ),
						new Variable( "testmethod1" )
					)
				),
				console_log(
					{
						new Expression(
							new Call(
								new Expression(
									new Variable( "testmethod" )
								),
								{
									new Expression(
										new program::Value( VALUE( type::Int, 1 ) )
									),
									new Expression(
										new Call(
											new Expression(
												new Variable( "testmethod" )
											),
											{
												new Expression(
													new program::Value( VALUE( type::Int, 2 ) )
												),
												new Expression(
													new Call(
														new Expression(
															new Variable( "testmethod" )
														),
														{
															new Expression(
																new program::Value( VALUE( type::Int, 3 ) )
															),
															new Expression(
																new program::Value( VALUE( type::Int, 3 ) )
															),
															new Expression(
																new program::Value( VALUE( type::Int, 3 ) )
															)
														}
													)
												),
												new Expression(
													new Call(
														new Expression(
															new Variable( "testmethod" )
														),
														{
															new Expression(
																new program::Value( VALUE( type::Int, 4 ) )
															),
															new Expression(
																new program::Value( VALUE( type::Int, 4 ) )
															),
															new Expression(
																new program::Value( VALUE( type::Int, 4 ) )
															)
														}
													)
												)
											}
										)
									),
									new Expression(
										new Call(
											new Expression(
												new Variable( "testmethod" )
											),
											{
												new Expression(
													new program::Value( VALUE( type::Int, 5 ) )
												),
												new Expression(
													new program::Value( VALUE( type::Int, 5 ) )
												),
												new Expression(
													new Call(
														new Expression(
															new Variable( "testmethod" )
														),
														{
															new Expression(
																new program::Value( VALUE( type::Int, 6 ) )
															),
															new Expression(
																new program::Value( VALUE( type::Int, 6 ) )
															),
															new Expression(
																new program::Value( VALUE( type::Int, 6 ) )
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
							new program::Value( VALUE( type::Int, 10 ) )
						)
					}
				),
				////
				console_log(
					{
						new Expression(
							new Variable( "testarr1" )
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "testarr2" )
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "testarr1" ),
							new Operator( Operator::OT_AT ),
							new program::Value( VALUE( type::Int, 0 ) )
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "testarr1" ),
							new Operator( Operator::OT_AT ),
							new program::Value( VALUE( type::Int, 1 ) )
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "testarr1" ),
							new Operator( Operator::OT_AT ),
							new Expression(
								new program::Value( VALUE( type::Int, 0 ) ),
								new Operator( Operator::OT_RANGE ),
								new program::Value( VALUE( type::Int, 1 ) )
							)
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "testarr1" ),
							new Operator( Operator::OT_AT ),
							new Expression(
								new program::Value( VALUE( type::Int, 5 ) ),
								new Operator( Operator::OT_RANGE ),
								nullptr
							)
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "testarr1" ),
							new Operator( Operator::OT_AT ),
							new Expression(
								nullptr,
								new Operator( Operator::OT_RANGE ),
								new program::Value( VALUE( type::Int, 3 ) )
							)
						)
					}
				),
				console_log(
					{
						new Expression(
							new Expression(
								new Variable( "testarr1" ),
								new Operator( Operator::OT_AT ),
								new Expression(
									new program::Value( VALUE( type::Int, 4 ) ),
									new Operator( Operator::OT_RANGE ),
									new program::Value( VALUE( type::Int, 5 ) )
								)
							),
							new Operator( Operator::OT_ADD ),
							new Expression(
								new Variable( "testarr1" ),
								new Operator( Operator::OT_AT ),
								new Expression(
									new program::Value( VALUE( type::Int, 2 ) ),
									new Operator( Operator::OT_RANGE ),
									new program::Value( VALUE( type::Int, 3 ) )
								)
							)
						)
					}
				),
				console_log(
					{
						new Expression(
							new Expression(
								new Expression(
									new Variable( "testobj3" ),
									new Operator( Operator::OT_CHILD ),
									new Variable( "child1" )
								),
								new Operator( Operator::OT_CHILD ),
								new Variable( "child2" )
							),
							new Operator( Operator::OT_CHILD ),
							new Variable( "value" )
						)
					}
				),
				console_log(
					{
						new Expression(
							new Expression(
								new Variable( "testobj1" ),
								new Operator( Operator::OT_CHILD ),
								new Variable( "propertyInt" )
							),
							new Operator( Operator::OT_EQ ),
							new Expression(
								new program::Value( VALUE( type::Int, 272 ) ),
								new Operator( Operator::OT_ADD ),
								new Variable( "c" )
							)
						)
					}
				),
				console_log(
					{
						new Expression(
							new Variable( "testobj1" )
						),
						new Expression(
							new Variable( "testobj2" )
						),
					}
				),
				console_log(
					{
						new Expression(
							new program::Value( VALUE( type::String, "bye!" ) )
						)
					}
				),
			}
		)
	);
	return &test_program;
}

}
