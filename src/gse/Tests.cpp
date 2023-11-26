#include "Tests.h"

#include "gse/type/Null.h"
#include "gse/type/Bool.h"
#include "gse/type/Int.h"

namespace gse {

void AddTests( task::gsetests::GSETests* task ) {

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

			class TestModuleY : public runnable::Runnable {
				Value Run( GSE* gse ) override {
					modules_run_order += 'Y';
					return VALUE( Null );
				}
			};
			NEWV( test_module_y, TestModuleY );
			gse.AddModule( "test_module_y", test_module_y );

			class TestModuleX : public runnable::Runnable {
				Value Run( GSE* gse ) override {
					modules_run_order += 'X';
					return VALUE( Null );
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
			class SetVariables : public runnable::Runnable {
				Value Run( GSE* gse ) override {
					gse->SetGlobal( "testvar_null", VALUE( Null ) );
					gse->SetGlobal( "testvar_bool_first", VALUE( Bool, true ) );
					gse->SetGlobal( "testvar_bool_second", VALUE( Bool, false ) );
					gse->SetGlobal( "testvar_int_first", VALUE( Int, 3 ) );
					gse->SetGlobal( "testvar_int_second", VALUE( Int, 7 ) );
					gse->SetGlobal( "testvar_int_third", VALUE( Int, 25 ) );
					return VALUE( Null );
				}
			};
			NEWV( set_variables, SetVariables );
			gse.AddModule( "set_variables", set_variables );

			static std::string errmsg = "";
			class CheckVariables : public runnable::Runnable {
			public:
				Value Run( GSE* gse ) override {
					const auto validate = [ gse ]() -> std::string {
						const type::Type* t;

#define CHECK( _varname, _expected_type ) \
                        t = gse->GetGlobal( _varname ).Get(); \
                        GT_ASSERT( t->type == _expected_type, "unexpected value type" );
#define CHECKV( _varname, _expected_type, _type, _expected_value ) \
                        CHECK( _varname, _expected_type ); \
                        GT_ASSERT( ((type::_type*)t)->value == _expected_value, "unexpected value" );

						CHECK( "testvar_undefined1", T_UNDEFINED );
						CHECK( "testvar_undefined2", T_UNDEFINED );
						CHECK( "testvar_undefined3", T_UNDEFINED );
						CHECK( "testvar_null", T_NULL );
						CHECKV( "testvar_bool_first", T_BOOL, Bool, true );
						CHECKV( "testvar_bool_second", T_BOOL, Bool, false );
						CHECKV( "testvar_int_first", T_INT, Int, 3 );
						CHECKV( "testvar_int_second", T_INT, Int, 7 );
						CHECKV( "testvar_int_third", T_INT, Int, 25 );

#undef CHECKV
#undef CHECK

						GT_OK();
					};
					errmsg = validate();
					return VALUE( Null );
				}
			};
			NEWV( check_variables, CheckVariables );
			gse.AddModule( "check_variables", check_variables );

			gse.Run();

			return errmsg;

		}
	);
}

}
