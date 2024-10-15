#include "Resource.h"

#include "ui/object/Mesh.h"
#include "scene/actor/Instanced.h"
#include "scene/actor/Sprite.h"
#include "types/mesh/Render.h"
#include "types/texture/Texture.h"
#include "game/frontend/base/Base.h"
#include "game/frontend/tile/Tile.h"
#include "game/frontend/Game.h"
#include "game/frontend/sprite/InstancedSpriteManager.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {
namespace center_area {

Resource::Resource( CenterArea* center_area )
	: CenterAreaPage( center_area ) {}

void Resource::Create() {
	CenterAreaPage::Create();
}

void Resource::Destroy() {
	HideTilePreview();

	CenterAreaPage::Destroy();
}

void Resource::Update( base::Base* base ) {
	PreviewTile( base->GetTile() );
}

void Resource::PreviewTile( const tile::Tile* tile ) {
	HideTilePreview();

	const auto& render = tile->GetRenderData();

	for ( auto& mesh : render.preview_meshes ) {

		NEWV( preview, ::ui::object::Mesh, "BPCenterAreaResourceTile" );
		NEWV( mesh_copy, types::mesh::Render, *mesh );
		preview->SetMesh( mesh_copy );
		preview->SetTexture( m_game->GetTerrainTexture() );
		m_preview_layers.push_back(
			{
				preview,
				nullptr
			}
		);

		AddChild( preview );
	}

	ASSERT( !m_preview_layers.empty(), "no preview layers defined" );

	// copy sprites from tile
	for ( auto& s : render.sprites ) {
		auto* actor = m_game->GetISM()->GetInstancedSpriteByKey( s )->actor;
		auto sprite = actor->GetSpriteActor();
		auto mesh = sprite->GenerateMesh();
		NEWV( sprite_preview, ::ui::object::Mesh, "BPCenterAreaResourceSprite" );
		sprite_preview->SetMesh( mesh );
		sprite_preview->SetTintColor(
			{
				0.7f,
				0.7f,
				0.7f,
				1.0f
			}
		); // lower brightness a bit because it's too high otherwise for some reason
		sprite_preview->SetTexture( sprite->GetTexture() );
		m_preview_layers.push_back(
			{
				sprite_preview,
				nullptr
			}
		);
		AddChild( sprite_preview );
	}
}

void Resource::HideTilePreview() {
	for ( auto& preview_layer : m_preview_layers ) {
		RemoveChild( preview_layer.object );
	}
	m_preview_layers.clear();
}

}
}
}
}
}
}
