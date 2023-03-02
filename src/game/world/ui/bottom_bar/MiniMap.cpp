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
		if ( m_texture ) {
			m_map_surface->SetTexture( m_texture );
		}
	AddChild( m_map_surface );

}

void MiniMap::Destroy() {
	
	RemoveChild( m_map_surface );
	
	Section::Destroy();
}

void MiniMap::SetMinimapTexture( types::Texture* texture ) {
	m_texture = texture;
	if ( m_map_surface ) {
		m_map_surface->SetTexture( m_texture );
	}
}

}
}
}
