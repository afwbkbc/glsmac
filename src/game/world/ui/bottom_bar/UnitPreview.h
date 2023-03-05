#pragma once

#include "Section.h"
#include "ui/object/Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( UnitPreview, Section )

	UnitPreview( World* world ) : Section( world, "UnitPreview" ) {}
	
	void Create();
	void Destroy();

};
	
}
}
}
