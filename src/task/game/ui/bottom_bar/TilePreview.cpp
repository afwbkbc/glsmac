#include "TilePreview.h"

#include "engine/Engine.h"
#include "../../Game.h"

namespace task {
namespace game {
namespace ui {

void TilePreview::Create() {
	BBSection::Create();
	
}

void TilePreview::Destroy() {
	HideTilePreview();
	
	BBSection::Destroy();
}

void TilePreview::PreviewTile( const tile_data_t& tile_data ) {
	HideTilePreview();
	
	for ( auto &mesh : tile_data.preview_meshes ) {		
		
		NEWV( preview, object::Mesh, "BBTilePreviewImage" );
			preview->SetMesh( mesh );
			preview->SetTexture( m_game->GetTerrainTexture() );
		m_preview_layers.push_back({
			preview,
			nullptr
		});
		
		AddChild( preview );
	}
	
	// print lines
	size_t label_top = m_preview_layers.front().object->GetHeight() + 6;
	for ( auto& line : tile_data.preview_lines ) {
		NEWV( label, object::Label, "BBTilePreviewTextLine" );
			label->SetText( line );
			label->SetTop( label_top );
		m_info_lines.push_back( label );
		AddChild( label );
		label_top += label->GetHeight();
	}
	
	// print tile coordinates
	NEWV( label, object::Label, "BBTilePreviewTextFooter" );
		label->SetText( "(" + std::to_string( tile_data.tile_position.x ) + "," + std::to_string( tile_data.tile_position.y ) + ")" );
	m_info_lines.push_back( label );
	AddChild( label );
	
	// copy sprites from tile
	for ( auto& s : tile_data.sprites ) {
		auto* actor = m_game->GetTerrainSpriteActorByKey( s );
		auto sprite = actor->GetSpriteActor();
		auto mesh = sprite->GenerateMesh();
		NEWV( sprite_preview, object::Mesh, "BBTilePreviewSprite" );
		sprite_preview->SetMesh( mesh );
		sprite_preview->SetTintColor({ 0.7f, 0.7f, 0.7f, 1.0f }); // lower brightness a bit because it's too high otherwise for some reason
		sprite_preview->SetTexture( sprite->GetTexture() );
		m_preview_layers.push_back({
			sprite_preview,
			nullptr
		});
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
