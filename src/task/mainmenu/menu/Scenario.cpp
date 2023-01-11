#include "Scenario.h"

using namespace game;

namespace task {
namespace mainmenu {

Scenario::Scenario( MainMenuTask *task ) : Menu( task, "", {
	{ "PLAY SCENARIO", [this] () -> void {
		// TODO
	}},
	{ "CREATE SCENARIO", [this] () -> void {
		// TODO
	}},
	{ "LOAD MAP FILE", [this] () -> void {
		// TODO
	}},
	{ "EDIT SCENARIO", [this] () -> void {
		// TODO
	}},
}) {}

}
}
