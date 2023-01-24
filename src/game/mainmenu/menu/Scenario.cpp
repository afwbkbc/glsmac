#include "Scenario.h"

using namespace game;

namespace game {
namespace mainmenu {

Scenario::Scenario( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "", {
	{ "PLAY SCENARIO", [this] () -> void {
		MenuError();
	}},
	{ "CREATE SCENARIO", [this] () -> void {
		MenuError();
	}},
	{ "LOAD MAP FILE", [this] () -> void {
		MenuError();
	}},
	{ "EDIT SCENARIO", [this] () -> void {
		MenuError();
	}},
}) {}

}
}
