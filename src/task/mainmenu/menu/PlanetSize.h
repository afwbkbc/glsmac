#pragma once

#include "../Menu.h"

namespace task {
namespace mainmenu {
	
class PlanetSize : public Menu {
public:
	PlanetSize( MainMenuTask *task );	
	void ChooseNext();
};

}
}
