#pragma once

#include "../SlidingMenu.h"

namespace game {
namespace mainmenu {
	
class PlanetSize : public SlidingMenu {
public:
	PlanetSize( MainMenu *mainmenu );	
	void ChooseNext();
};

}
}
