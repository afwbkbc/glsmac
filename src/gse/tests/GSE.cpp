#include "GSE.h"

#include "task/gsetests/GSETests.h"
#include "gse/GSE.h"
#include "gse/value/Null.h"
#include "gse/value/Bool.h"
#include "gse/value/Int.h"
#include "gse/value/String.h"
#include "gse/value/Object.h"
#include "gse/value/Callable.h"
#include "gse/ExecutionPointer.h"
#include "gc/Space.h"

namespace gse {
namespace tests {

void AddGSETests( task::gsetests::GSETests* task ) {
	
	class Sum : public value::Callable {
	public:
		Sum( gc::Space* const gc_space, context::Context* const ctx )
			: value::Callable( gc_space, ctx ) {}
		Value* Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) override {
			int64_t result = 0;
			for ( const auto& it : arguments ) {
				const auto arg = it;
				switch ( arg->type ) {
					case Value::T_INT: {
						result += ( (value::Int*)arg )->value;
						break;
					}
					default: {
						// TODO: exception
					}
				}
			}
			return VALUE( value::Int, , result );
		}
	};
	
	task->AddTest(
		"test if modules get assigned and run correctly",
		GT() {
			auto* gc_space = gse->GetGCSpace();
			
			static std::string modules_run_order = "";
			
			class TestModuleY : public value::Callable {
			public:
				TestModuleY( gc::Space* const gc_space, context::Context* const ctx )
					: Callable( gc_space, ctx ) {}
				Value* Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) override {
					modules_run_order += 'Y';
					return VALUE( value::Null );
				}
			};
			gc_space->Accumulate(
				nullptr,
				[ &gc_space, &ctx, &gse ]() {
					NEWV( test_module_y, TestModuleY, gc_space, ctx );
					gse->AddModule( "test_module_y", test_module_y );
				}
			);
			
			class TestModuleX : public value::Callable {
			public:
				TestModuleX( gc::Space* const gc_space, context::Context* const ctx )
					: Callable( gc_space, ctx ) {}
				Value* Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) override {
					modules_run_order += 'X';
					return VALUE( value::Null );
				}
			};
			gc_space->Accumulate(
				nullptr,
				[ &gc_space, &ctx, &gse ]() {
					NEWV( test_module_x, TestModuleX, gc_space, ctx );
					gse->AddModule( "test_module_x", test_module_x );
				}
			);
			
			gse->Run();
			
			GT_ASSERT( modules_run_order == "YX", "wrong modules order" );
			
			GT_OK();
		}
	);
	
	task->AddTest(
		"test if variables are written and read correctly",
		GT() {
			auto* gc_space = gse->GetGCSpace();
			
			class SetVariables : public value::Callable {
			public:
				SetVariables( gc::Space* const gc_space, GSE* gse, context::Context* const ctx )
					: value::Callable( gc_space, ctx )
					, gse( gse ) {}
				GSE* gse;
				Value* Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) override {
					gse->SetGlobal( "testvar_null", VALUE( value::Null ) );
					gse->SetGlobal( "testvar_bool_first", VALUE( value::Bool, , true ) );
					gse->SetGlobal( "testvar_bool_second", VALUE( value::Bool, , false ) );
					gse->SetGlobal( "testvar_int_first", VALUE( value::Int, , 3 ) );
					gse->SetGlobal( "testvar_int_second", VALUE( value::Int, , 7 ) );
					gse->SetGlobal( "testvar_int_third", VALUE( value::Int, , 25 ) );
					gse->SetGlobal( "testvar_string_first", VALUE( value::String, , "FIRST" ) );
					gse->SetGlobal( "testvar_string_second", VALUE( value::String, , "SECOND" ) );
					
					return VALUE( value::Null );
				}
			};
			gc_space->Accumulate(
				nullptr,
				[ &gc_space, &ctx, &gse ]() {
					NEWV( set_variables, SetVariables, gc_space, gse, ctx );
					gse->AddModule( "set_variables", set_variables );
				}
			);
			
			static std::string errmsg = "";
			class CheckVariables : public value::Callable {
			public:
				CheckVariables( gc::Space* const gc_space, GSE* gse, context::Context* const ctx )
					: value::Callable( gc_space, ctx )
					, gse( gse ) {}
				GSE* gse;
				Value* Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) override {
					const auto validate = [ this ]() -> const std::string {
						Value* t;

#define CHECK( _varname, _expected_type ) \
                        t = gse->GetGlobal( _varname ); \
                        GT_ASSERT( t->type == _expected_type, "unexpected value type" );
#define CHECKV( _varname, _type, _expected_value ) \
                        CHECK( _varname, value::_type::GetType() ); \
                        GT_ASSERT( ((value::_type*)t)->value == _expected_value, "unexpected value" );
						
						CHECK( "testvar_undefined1", T_UNDEFINED );
						CHECK( "testvar_undefined2", T_UNDEFINED );
						CHECK( "testvar_undefined3", T_UNDEFINED );
						CHECK( "testvar_null", T_NULL );
						CHECKV( "testvar_bool_first", Bool, true );
						CHECKV( "testvar_bool_second", Bool, false );
						CHECKV( "testvar_int_first", Int, 3 );
						CHECKV( "testvar_int_second", Int, 7 );
						CHECKV( "testvar_int_third", Int, 25 );
						CHECKV( "testvar_string_first", String, "FIRST" );
						CHECKV( "testvar_string_second", String, "SECOND" );
#undef CHECKV
#undef CHECK
						GT_OK();
					};
					errmsg = validate();
					return VALUE( value::Null );
				}
			};
			gc_space->Accumulate(
				nullptr,
				[ &gc_space, &ctx, &gse ]() {
					NEWV( check_variables, CheckVariables, gc_space, gse, ctx );
					gse->AddModule( "check_variables", check_variables );
				}
			);
			
			gse->Run();
			
			return errmsg;
			
		}
	);
	
	task->AddTest(
		"test if methods are defined and called correctly",
		GT() {
			auto* gc_space = gse->GetGCSpace();
			
			static bool wasTestMethodCalled = false;
			
			class SetMethods : public value::Callable {
			public:
				SetMethods( gc::Space* const gc_space, GSE* gse, context::Context* const ctx )
					: value::Callable( gc_space, ctx )
					, gse( gse ) {}
				GSE* gse;
				
				class TestMethod : public value::Callable {
				public:
					TestMethod( gc::Space* const gc_space, context::Context* const ctx )
						: value::Callable( gc_space, ctx ) {}
					Value* Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) override {
						wasTestMethodCalled = true;
						return VALUE( value::Null );
					}
				};
				
				Value* Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) override {
					gse->SetGlobal( "test_method", VALUE( TestMethod, , ctx ) );
					gse->SetGlobal( "sum", VALUE( Sum, , ctx ) );
					return VALUE( value::Null );
				}
			};
			gc_space->Accumulate(
				nullptr,
				[ &gc_space, &ctx, &gse ]() {
					NEWV( set_methods, SetMethods, gc_space, gse, ctx );
					gse->AddModule( "set_methods", set_methods );
				}
			);
			
			gse->Run();

#define GETMETHOD( _method ) \
    auto* value = gse->GetGlobal( _method ); \
    GT_ASSERT( value->type != Value::T_UNDEFINED, "method not found" ); \
    GT_ASSERT( value->type == Value::T_CALLABLE, "method is not callable" ); \
    auto* method = (value::Callable*)value;

#define GETRESULT( _args, _expected_type, _cast_type ) \
    ExecutionPointer ep; \
    auto resultvalue = method->Run( gc_space, nullptr, {}, ep, _args ); \
    auto* resultv = resultvalue; \
    GT_ASSERT( resultv->type == _expected_type, "wrong result type ( " + std::to_string( resultv->type ) + " != " + std::to_string( _expected_type ) + " )" ); \
    auto* result = (_cast_type*)resultv;

#define CHECKRESULT( _args, _expected_type, _cast_type, _expected_value ) \
    { \
        GETRESULT( _args, _expected_type, _cast_type ); \
        GT_ASSERT( result->value == _expected_value, "wrong result value: " + std::to_string( result->value ) ); \
    }
			
			const auto& run_tests = [ &gse, &gc_space ]() -> std::string {
				{ // test test_method()
					GETMETHOD( "test_method" );
					GT_ASSERT( !wasTestMethodCalled, "method should have not be called yet" );
					GETRESULT( {}, Value::T_NULL, value::Null );
					GT_ASSERT( wasTestMethodCalled, "method should have been called" );
				}
				{ // test sum()
					GETMETHOD( "sum" );
					CHECKRESULT( {}, Value::T_INT, value::Int, 0 );
					CHECKRESULT( { VALUE( value::Int, , 3 ) }, Value::T_INT, value::Int, 3 );
					std::vector< Value* > args = {
						VALUE( value::Int, , 2 ),
						VALUE( value::Int, , 5 ),
					};
					CHECKRESULT( args, Value::T_INT, value::Int, 7 );
					args = {
						VALUE( value::Int, , 11 ),
						VALUE( value::Int, , 22 ),
						VALUE( value::Int, , 33 ),
					};
					CHECKRESULT( args, Value::T_INT, value::Int, 66 );
				}
				GT_OK();
			};

#undef GETMETHOD
#undef GETRESULT
#undef CHECKRESULT
			
			std::string result = "";
			gc_space->Accumulate(
				nullptr,
				[ &run_tests, &result ]() {
					result = run_tests();
				}
			);
			
			return result;
		}
	);
	
	task->AddTest(
		"test if variables are assigned and reassigned correctly",
		GT() {
			auto* gc_space = gse->GetGCSpace();
			
			class SetVariables : public value::Callable {
			public:
				SetVariables( gc::Space* const gc_space, GSE* gse, context::Context* const ctx )
					: value::Callable( gc_space, ctx )
					, gse( gse ) {}
				GSE* gse;
				Value* Run( GSE_CALLABLE, const value::function_arguments_t& arguments ) override {
					
					auto val1 = VALUE( value::Int, , 1 );
					auto val2 = VALUE( value::Int, , VALUE_GET( value::Int, val1 ) + 1 );
					
					gse->SetGlobal( "testvar_first", val1 );
					gse->SetGlobal( "testvar_second", val2 );
					
					VALUE_SET( value::Int, val1, 10 ); // this should update testvar_first
					
					auto val3 = VALUE_CLONE( value::Int, val2 );
					VALUE_SET( value::Int, val3, 20 ); // this should not update testvar_second
					gse->SetGlobal( "testvar_third", val3 );
					
					gse->SetGlobal( "testvar_obj1", VALUEEXT( value::Object, GSE_CALL ) );
					
					auto obj2 = VALUEEXT( value::Object, GSE_CALL );
					auto data2 = VALUE_DATA( value::Object, obj2 );
					data2->Set( "property_int", VALUE( value::Int, , 555 ), GSE_CALL );
					data2->Set( "property_bool", VALUE( value::Bool, , false ), GSE_CALL );
					data2->Set( "property_bool", VALUE( value::Bool, , true ), GSE_CALL ); // this should overwrite previous value
					gse->SetGlobal( "testvar_obj2", obj2 );
					
					auto properties = value::object_properties_t{
						{ "property_int", val3 },
						{ "property_sum",    VALUE( Sum, , ctx ) },
						{ "property_string", VALUE( value::String, , "STRING" ) },
					};
					gse->SetGlobal( "testvar_obj3", VALUEEXT( value::Object, GSE_CALL, properties ) );
					
					VALUE_SET( value::Int, val3, 30 ); // this should update testvar_third and testvar_obj3.property_int
					
					return VALUE( value::Null );
				}
			};
			
			std::string result = "";
			
			gc_space->Accumulate(
				nullptr,
				[ &gc_space, &ctx, &gse, &result ]() {
					NEWV( set_variables, SetVariables, gc_space, gse, ctx );
					gse->AddModule( "set_variables", set_variables );
					
					gse->Run();
					
					const auto f_asserts = [ &gse, &gc_space ]() {
						
						GT_ASSERT( VALUE_GET( value::Int, gse->GetGlobal( "testvar_first" ) ) == 10 );
						GT_ASSERT( VALUE_GET( value::Int, gse->GetGlobal( "testvar_second" ) ) == 2 );
						GT_ASSERT( VALUE_GET( value::Int, gse->GetGlobal( "testvar_third" ) ) == 30 );
						
						const auto obj1 = VALUE_GET( value::Object, gse->GetGlobal( "testvar_obj1" ) );
						GT_ASSERT( obj1.empty() );
						
						const auto obj2 = VALUE_GET( value::Object, gse->GetGlobal( "testvar_obj2" ) );
						GT_ASSERT( obj2.size() == 2 );
						GT_ASSERT( VALUE_GET( value::Int, obj2.at( "property_int" ) ) == 555 );
						GT_ASSERT( VALUE_GET( value::Bool, obj2.at( "property_bool" ) ) == true );
						
						const auto obj3 = VALUE_GET( value::Object, gse->GetGlobal( "testvar_obj3" ) );
						GT_ASSERT( obj3.size() == 3 );
						GT_ASSERT( VALUE_GET( value::Int, obj3.at( "property_int" ) ) == 30 );
						GT_ASSERT( VALUE_GET( value::String, obj3.at( "property_string" ) ) == "STRING" );
						const auto sum = VALUE_DATA( value::Callable, obj3.at( "property_sum" ) );
						
						const auto run_tests = [ &gse, &gc_space, &sum ]() -> const std::string {
							std::vector< Value* > args = {
								VALUE( value::Int, , 2 ),
								VALUE( value::Int, , 5 ),
							};
							{
								ExecutionPointer ep;
								GT_ASSERT( VALUE_GET( value::Int, sum->Run( gc_space, nullptr, {}, ep, args ) ) == 7 );
							}
							GT_OK();
						};
						
						return run_tests();
					};
					
					result = f_asserts();
					
				}
			);
			return result;
		}
	);
	
}

}
}
