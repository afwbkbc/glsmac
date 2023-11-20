#include "Tests.h"

namespace world {

void AddTests( task::worldtests::WorldTests* task ) {
	task->AddTest(
		"test if tests work",
		WT() {
			return "";
		}
	);
}

}