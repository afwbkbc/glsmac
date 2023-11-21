#include "Tests.h"

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

			class TestModuleX : public runnable::Runnable {
				type::Type& Run( GSE* gse ) override {
					modules_run_order += 'X';
					return nullptr;
				}
			};
			NEWV( test_module_x, TestModuleX );
			class TestModuleY : public runnable::Runnable {
				type::Type& Run( GSE* gse ) override {
					modules_run_order += 'Y';
					return nullptr;
				}
			};
			NEWV( test_module_y, TestModuleY );

			gse.AddModule( "test_module_y", test_module_y );
			gse.AddModule( "test_module_x", test_module_x );

			gse.Run();

			GT_ASSERT( modules_run_order == "YX", "wrong modules order" );

			GT_OK();
		}
	);
}

}
