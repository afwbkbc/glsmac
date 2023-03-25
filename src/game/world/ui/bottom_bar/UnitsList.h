#pragma once

#include "BBSection.h"
#include "ui/object/Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( UnitsList, BBSection )

	UnitsList( World* world ) : BBSection( world, "UnitsList" ) {}
	
	void Create();
	void Destroy();

};
	
}
}
}
