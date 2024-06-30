#pragma once

#include "task/mainmenu/SlidingMenu.h"

namespace task {
namespace mainmenu {

CLASS( PlanetSize, SlidingMenu )
	PlanetSize( MainMenu* mainmenu );
	void ChooseNext();
};

}
}
