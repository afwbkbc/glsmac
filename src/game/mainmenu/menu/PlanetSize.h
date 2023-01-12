#pragma once

#include "Menu.h"

namespace game {
namespace mainmenu {
	
class PlanetSize : public Menu {
public:
	PlanetSize( MainMenu *mainmenu );	
	void ChooseNext();
};

}
}
