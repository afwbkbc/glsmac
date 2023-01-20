#include "Scenario.h"

using namespace game;

namespace game {
namespace mainmenu {

Scenario::Scenario( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "", {
	{ "PLAY SCENARIO", [this] () -> void {
		MenuError( "This feature is not available yet." );
	}},
	{ "CREATE SCENARIO", [this] () -> void {
		MenuError( "This feature is not available yet." );
	}},
	{ "LOAD MAP FILE", [this] () -> void {
		MenuError( "This feature is not available yet." );
	}},
	{ "EDIT SCENARIO", [this] () -> void {
		MenuError( "This feature is not available yet." );
	}},
}) {}

}
}
