#include "Tests.h"

namespace world {

void AddTests( task::worldtests::WorldTests* task ) {

	task->AddTest(
		"test if tests work",
		WT() {
			return "";
		}
	);

	task->AddTest(
		"test if modules get assigned and run correctly",
		WT() {

			static std::string modules_run_order = "";

			class TestModuleX : public world::runnable::Runnable {
				void Run() override {
					modules_run_order += 'X';
				}
			};
			NEWV( test_module_x, TestModuleX );
			class TestModuleY : public world::runnable::Runnable {
				void Run() override {
					modules_run_order += 'Y';
				}
			};
			NEWV( test_module_y, TestModuleY );

			world.AddModule( "test_module_y", test_module_y );
			world.AddModule( "test_module_x", test_module_x );

			world.Run();

			WT_ASSERT( modules_run_order == "YX", "wrong modules order" );

			WT_OK();
		}
	);
}

}
