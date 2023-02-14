#pragma once

#include "Section.h"
#include "ui/object/Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( InfoPanel, Section )

	InfoPanel( const World* world ) : Section( world, "InfoPanel" ) {}
	
	void Create();
	void Destroy();

};
	
}
}
}
