#pragma once

#include "Section.h"
#include "ui/object/Section.h"

#include "ui/object/Mesh.h"

namespace game {
namespace world {
namespace ui {

CLASS( MiniMap, Section )

	MiniMap( const World* world ) : Section( world, "MiniMap" ) {}
	
	void Create();
	void Destroy();

private:
	object::Mesh* m_map_surface = nullptr;
	
};
	
}
}
}
