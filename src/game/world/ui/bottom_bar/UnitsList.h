#pragma once

#include "Section.h"
#include "ui/object/Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( UnitsList, Section )

	UnitsList( World* world ) : Section( world, "UnitsList" ) {}
	
	void Create();
	void Destroy();

};
	
}
}
}
