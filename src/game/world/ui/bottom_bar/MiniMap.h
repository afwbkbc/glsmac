#pragma once

#include "Section.h"
#include "ui/object/Section.h"

#include "ui/object/Mesh.h"

#include "types/Texture.h"

namespace game {
namespace world {
namespace ui {

CLASS( MiniMap, Section )

	MiniMap( const World* world ) : Section( world, "MiniMap" ) {}
	
	void Create();
	void Destroy();

	void SetMinimapTexture( types::Texture* texture );
	
private:
	types::Texture* m_texture = nullptr;
	object::Mesh* m_map_surface = nullptr;
	
};
	
}
}
}
