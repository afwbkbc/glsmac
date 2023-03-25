#pragma once

#include "BBSection.h"
#include "ui/object/Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( UnitPreview, BBSection )

	UnitPreview( World* world ) : BBSection( world, "UnitPreview" ) {}
	
	void Create();
	void Destroy();

};
	
}
}
}
