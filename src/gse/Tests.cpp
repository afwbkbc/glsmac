#include "Tests.h"

#include "tests/GSE.h"
#include "tests/Parser.h"
#include "tests/Runner.h"

#include "gse/program/Variable.h"
#include "gse/program/Value.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"

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
						new Call(
							new Expression(
								new Variable( "console" ),
								new Operator( Operator::OT_CHILD ),
								new Variable( "log" )
							),
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
									new Operator( Operator::OT_EQ ),
									new Expression(
										new program::Value( VALUE( type::Int, 332 ) ),
										new Operator( Operator::OT_ADD ),
										new Variable( "c" )
									)
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
								new Expression(
									new Variable( "testobj1" )
								),
								new Expression(
									new Variable( "testobj2" )
								),
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
								new Expression(
									new program::Value( VALUE( type::String, "bye!" ) )
								)
							}
						)
					)
				),
			}
		)
	);
	return &test_program;
}

}
