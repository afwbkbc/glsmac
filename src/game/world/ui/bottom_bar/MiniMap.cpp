#include "MiniMap.h"

#include "engine/Engine.h"
#include "../../World.h"


namespace game {
namespace world {
namespace ui {

void MiniMap::Create() {
	Section::Create();

	NEW( m_map_surface, object::Mesh, "MapBottomBarMinimapImage" );
		m_map_surface->SetMesh( types::mesh::Render::Rectangle() );
		m_map_surface->SetTexture( m_world->GetMap()->GetMinimapTexture() );
	AddChild( m_map_surface );

}

void MiniMap::Destroy() {
	
	RemoveChild( m_map_surface );
	
	Section::Destroy();
}

}
}
}
