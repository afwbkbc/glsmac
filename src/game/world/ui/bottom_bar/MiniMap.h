#pragma once

#include "Section.h"
#include "ui/object/Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( MiniMap, Section )

	MiniMap( const World* world ) : Section( world, "MiniMap" ) {}
	
	void Create();
	void Destroy();

};
	
}
}
}
