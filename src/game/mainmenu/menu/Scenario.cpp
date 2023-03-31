#include "Scenario.h"

using namespace game;

namespace game {
namespace mainmenu {

Scenario::Scenario( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "", {
	{ "PLAY SCENARIO", {} },
	{ "CREATE SCENARIO", {} },
	{ "LOAD MAP FILE", {} },
	{ "EDIT SCENARIO", {} }
}) {}

}
}
