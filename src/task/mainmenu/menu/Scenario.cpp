#include "Scenario.h"

using namespace task;

namespace task {
namespace mainmenu {

Scenario::Scenario( MainMenu* mainmenu )
	: SlidingMenu(
	mainmenu, "", {
		{ "PLAY SCENARIO",   {} },
		{ "CREATE SCENARIO", {} },
		{ "LOAD MAP FILE",   {} },
		{ "EDIT SCENARIO",   {} }
	}
) {}

}
}
