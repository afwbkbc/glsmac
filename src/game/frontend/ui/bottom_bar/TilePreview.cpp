#include "TilePreview.h"

#include "engine/Engine.h"
#include "game/frontend/Game.h"
#include "game/frontend/tile/Tile.h"
#include "ui/object/Mesh.h"
#include "types/mesh/Render.h"
#include "ui/object/Label.h"
#include "ui/object/Mesh.h"
#include "game/frontend/sprite/InstancedSpriteManager.h"
#include "game/frontend/sprite/InstancedSprite.h"
#include "scene/actor/Instanced.h"
#include "scene/actor/Sprite.h"

namespace game {
namespace frontend {
namespace ui {

void TilePreview::Create() {
	BBSection::Create();

}

void TilePreview::Destroy() {
	HideTilePreview();

	BBSection::Destroy();
}

void TilePreview::PreviewTile( const tile::Tile* tile ) {
	HideTilePreview();

	const auto& render = tile->GetRenderData();

	for ( auto& mesh : render.preview_meshes ) {

		NEWV( preview, ::ui::object::Mesh, "BBTilePreviewImage" );
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

	// print lines
	size_t label_top = m_preview_layers.front().object->GetHeight() + 6;
	for ( auto& line : render.preview_lines ) {
		NEWV( label, ::ui::object::Label, "BBTilePreviewTextLine" );
		label->SetText( line );
		label->SetTop( label_top );
		m_info_lines.push_back( label );
		AddChild( label );
		label_top += label->GetHeight();
	}

	// print tile coordinates
	NEWV( label, ::ui::object::Label, "BBTilePreviewTextFooter" );
	label->SetText( "(" + std::to_string( tile->GetCoords().x ) + "," + std::to_string( tile->GetCoords().y ) + ")" );
	m_info_lines.push_back( label );
	AddChild( label );

	// copy sprites from tile
	for ( auto& s : render.sprites ) {
		auto* actor = m_game->GetISM()->GetInstancedSpriteByKey( s )->actor;
		auto sprite = actor->GetSpriteActor();
		auto mesh = sprite->GenerateMesh();
		NEWV( sprite_preview, ::ui::object::Mesh, "BBTilePreviewSprite" );
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

void TilePreview::HideTilePreview() {
	for ( auto& label : m_info_lines ) {
		RemoveChild( label );
	}
	m_info_lines.clear();
	for ( auto& preview_layer : m_preview_layers ) {
		RemoveChild( preview_layer.object );
	}
	m_preview_layers.clear();
}

}
}
}
