#include "Tests.h"

#include "gse/type/Null.h"
#include "gse/type/Bool.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"
#include "gse/type/Callable.h"

#include "gse/type/Callable.h"

namespace gse {

void AddTests( task::gsetests::GSETests* task ) {

	class Sum : public type::Callable {
		Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override {
			int32_t result = 0;
			for ( const auto& it : arguments ) {
				const auto arg = it.Get();
				switch ( arg->type ) {
					case type::Type::T_INT: {
						result += ( (type::Int*)arg )->value;
						break;
					}
					default: {
						// TODO: exception
					}
				}
			}
			return VALUE( type::Int, result );
		}
	};

	task->AddTest(
		"test if tests work",
		GT() {
			return "";
		}
	);

	task->AddTest(
		"test if modules get assigned and run correctly",
		GT() {

			static std::string modules_run_order = "";

			class TestModuleY : public type::Callable {
				Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override {
					modules_run_order += 'Y';
					return VALUE( type::Null );
				}
			};
			NEWV( test_module_y, TestModuleY );
			gse.AddModule( "test_module_y", test_module_y );

			class TestModuleX : public type::Callable {
				Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override {
					modules_run_order += 'X';
					return VALUE( type::Null );
				}
			};
			NEWV( test_module_x, TestModuleX );
			gse.AddModule( "test_module_x", test_module_x );

			gse.Run();

			GT_ASSERT( modules_run_order == "YX", "wrong modules order" );

			GT_OK();
		}
	);

	task->AddTest(
		"test if variables are written and read correctly",
		GT() {
			class SetVariables : public type::Callable {
				Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override {
					gse->SetGlobal( "testvar_null", VALUE( type::Null ) );
					gse->SetGlobal( "testvar_bool_first", VALUE( type::Bool, true ) );
					gse->SetGlobal( "testvar_bool_second", VALUE( type::Bool, false ) );
					gse->SetGlobal( "testvar_int_first", VALUE( type::Int, 3 ) );
					gse->SetGlobal( "testvar_int_second", VALUE( type::Int, 7 ) );
					gse->SetGlobal( "testvar_int_third", VALUE( type::Int, 25 ) );
					gse->SetGlobal( "testvar_string_first", VALUE( type::String, "FIRST" ) );
					gse->SetGlobal( "testvar_string_second", VALUE( type::String, "SECOND" ) );

					return VALUE( type::Null );
				}
			};
			NEWV( set_variables, SetVariables );
			gse.AddModule( "set_variables", set_variables );

			static std::string errmsg = "";
			class CheckVariables : public type::Callable {
			public:
				Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override {
					const auto validate = [ gse ]() -> std::string {
						const type::Type* t;

#define CHECK( _varname, _expected_type ) \
                        t = gse->GetGlobal( _varname ).Get(); \
                        GT_ASSERT( t->type == _expected_type, "unexpected value type" );
#define CHECKV( _varname, _type, _expected_value ) \
                        CHECK( _varname, type::_type::GetType() ); \
                        GT_ASSERT( ((type::_type*)t)->value == _expected_value, "unexpected value" );

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
					return VALUE( type::Null );
				}
			};
			NEWV( check_variables, CheckVariables );
			gse.AddModule( "check_variables", check_variables );

			gse.Run();

			return errmsg;

		}
	);

	task->AddTest(
		"test if methods are defined and called correctly",
		GT() {

			static bool wasTestMethodCalled = false;

			class SetMethods : public type::Callable {

				class TestMethod : public type::Callable {
					Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override {
						wasTestMethodCalled = true;
						return VALUE( type::Null );
					}
				};

				Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override {
					gse->SetGlobal( "test_method", VALUE( TestMethod ) );
					gse->SetGlobal( "sum", VALUE( Sum ) );
					return VALUE( type::Null );
				}
			};
			NEWV( set_methods, SetMethods );
			gse.AddModule( "set_methods", set_methods );

			gse.Run();

#define GETMETHOD( _method ) \
    auto* value = gse.GetGlobal( _method ).Get(); \
    GT_ASSERT( value->type != type::Type::T_UNDEFINED, "method not found" ); \
    GT_ASSERT( value->type == type::Type::T_CALLABLE, "method is not callable" ); \
    auto* method = (type::Callable*)value;

#define GETRESULT( _args, _expected_type, _cast_type ) \
    auto* resultv = method->Run( &gse, _args ).Get(); \
    GT_ASSERT( resultv->type == _expected_type, "wrong result type" ); \
    auto* result = (_cast_type*)resultv;

#define CHECKRESULT( _args, _expected_type, _cast_type, _expected_value ) \
    { \
        GETRESULT( _args, _expected_type, _cast_type ); \
        GT_ASSERT( result->value == _expected_value, "wrong result value: " + std::to_string( result->value ) ); \
    }

			{ // test test_method()
				GETMETHOD( "test_method" );
				GT_ASSERT( !wasTestMethodCalled, "method should have not be called yet" );
				GETRESULT( {}, type::Type::T_NULL, type::Null );
				GT_ASSERT( wasTestMethodCalled, "method should have been called" );
			}
			{ // test sum()
				GETMETHOD( "sum" );
				CHECKRESULT( {}, type::Type::T_INT, type::Int, 0 );
				CHECKRESULT( { VALUE( type::Int, 3 ) }, type::Type::T_INT, type::Int, 3 );
				std::vector< Value > args = {
					VALUE( type::Int, 2 ),
					VALUE( type::Int, 5 ),
				};
				CHECKRESULT( args, type::Type::T_INT, type::Int, 7 );
				args = {
					VALUE( type::Int, 11 ),
					VALUE( type::Int, 22 ),
					VALUE( type::Int, 33 ),
				};
				CHECKRESULT( args, type::Type::T_INT, type::Int, 66 );
			}

#undef GETMETHOD
#undef GETRESULT
#undef CHECKRESULT

			GT_OK();
		}
	);

	task->AddTest(
		"test if variables are assigned and reassigned correctly",
		GT() {
			class SetVariables : public type::Callable {
				Value Run( GSE* gse, const Callable::function_arguments_t arguments ) override {

					auto val1 = VALUE( type::Int, 1 );
					auto val2 = VALUE( type::Int, VALUE_GET( type::Int, val1 ) + 1 );

					gse->SetGlobal( "testvar_first", val1 );
					gse->SetGlobal( "testvar_second", val2 );

					VALUE_SET( type::Int, val1, 10 ); // this should update testvar_first

					auto val3 = VALUE_CLONE( type::Int, val2 );
					VALUE_SET( type::Int, val3, 20 ); // this should not update testvar_second
					gse->SetGlobal( "testvar_third", val3 );

					gse->SetGlobal( "testvar_obj1", VALUE( type::Object ) );

					auto obj2 = VALUE( type::Object );
					auto data2 = VALUE_DATA( type::Object, obj2 );
					data2->Set( "property_int", VALUE( type::Int, 555 ) );
					data2->Set( "property_bool", VALUE( type::Bool, false ) );
					data2->Set( "property_bool", VALUE( type::Bool, true ) ); // this should overwrite previous value
					gse->SetGlobal( "testvar_obj2", obj2 );

					auto properties = type::Object::properties_t{
						{ "property_int", val3 },
						{ "property_sum",    VALUE( Sum ) },
						{ "property_string", VALUE( type::String, "STRING" ) },
					};
					gse->SetGlobal( "testvar_obj3", VALUE( type::Object, properties ) );

					VALUE_SET( type::Int, val3, 30 ); // this should update testvar_third and testvar_obj3.property_int

					return VALUE( type::Null );
				}
			};
			NEWV( set_variables, SetVariables );
			gse.AddModule( "set_variables", set_variables );

			gse.Run();

			GT_ASSERT( VALUE_GET( type::Int, gse.GetGlobal( "testvar_first" ) ) == 10 );
			GT_ASSERT( VALUE_GET( type::Int, gse.GetGlobal( "testvar_second" ) ) == 2 );
			GT_ASSERT( VALUE_GET( type::Int, gse.GetGlobal( "testvar_third" ) ) == 30 );

			const auto obj1 = VALUE_GET( type::Object, gse.GetGlobal( "testvar_obj1" ) );
			GT_ASSERT( obj1.empty() );

			const auto obj2 = VALUE_GET( type::Object, gse.GetGlobal( "testvar_obj2" ) );
			GT_ASSERT( obj2.size() == 2 );
			GT_ASSERT( VALUE_GET( type::Int, obj2.at( "property_int" ) ) == 555 );
			GT_ASSERT( VALUE_GET( type::Bool, obj2.at( "property_bool" ) ) == true );

			const auto obj3 = VALUE_GET( type::Object, gse.GetGlobal( "testvar_obj3" ) );
			GT_ASSERT( obj3.size() == 3 );
			GT_ASSERT( VALUE_GET( type::Int, obj3.at( "property_int" ) ) == 30 );
			GT_ASSERT( VALUE_GET( type::String, obj3.at( "property_string" ) ) == "STRING" );
			const auto sum = VALUE_DATA( type::Callable, obj3.at( "property_sum" ) );
			std::vector< Value > args = {
				VALUE( type::Int, 2 ),
				VALUE( type::Int, 5 ),
			};
			GT_ASSERT( VALUE_GET( type::Int, sum->Run( &gse, args ) ) == 7 );

			GT_OK();
		}
	);

}

}
