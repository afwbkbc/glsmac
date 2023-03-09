#pragma once

#include "Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( InfoPanel, Section )

	InfoPanel( World* world ) : Section( world, "InfoPanel" ) {}
	
	void Create();
	void Destroy();

};
	
}
}
}
