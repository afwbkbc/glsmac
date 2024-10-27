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

	NEW( m_pages.normal, ::ui::object::UIContainer, "BBTilePreviewPage" );
	if ( show_yields_page ) {
		m_pages.normal->Hide();
	}
	AddChild( m_pages.normal );

	// print lines
	size_t label_top = 2;
	for ( auto& line : render.preview_lines ) {
		NEWV( label, ::ui::object::Label, "BBTilePreviewTextLine" );
		label->SetText( line );
		label->SetTop( label_top );
		m_texts.normal.push_back( label );
		m_pages.normal->AddChild( label );
		label_top += label->GetHeight();
	}

	// yields page
	NEW( m_pages.yields, ::ui::object::UIContainer, "BBTilePreviewPage" );
	if ( !show_yields_page ) {
		m_pages.yields->Hide();
	}
	AddChild( m_pages.yields );

	// print tile coordinates
	NEWV( label, ::ui::object::Label, "BBTilePreviewTextFooter" );
	label->SetText( "(" + std::to_string( tile->GetCoords().x ) + "," + std::to_string( tile->GetCoords().y ) + ")" );
	m_texts.footer = label;
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

	On(
		::ui::event::EV_MOUSE_DOWN, EH( this ) {
			if ( !show_yields_page ) {
				show_yields_page = true;
				m_pages.normal->Hide();
				m_pages.yields->Show();
			}
			else {
				show_yields_page = false;
				m_pages.yields->Hide();
				m_pages.normal->Show();
			}
			return true;
		}
	);

}

void TilePreview::HideTilePreview() {
	if ( m_pages.normal ) {
		for ( auto& label : m_texts.normal ) {
			m_pages.normal->RemoveChild( label );
		}
		m_texts.normal.clear();
		RemoveChild( m_pages.normal );
		m_pages.normal = nullptr;
	}

	if ( m_pages.yields ) {
		HideYields();
		RemoveChild( m_pages.yields );
		m_pages.yields = nullptr;
	}

	if ( m_texts.footer ) {
		RemoveChild( m_texts.footer );
		m_texts.footer = nullptr;
	}

	for ( auto& preview_layer : m_preview_layers ) {
		RemoveChild( preview_layer.object );
	}
	m_preview_layers.clear();
}

void TilePreview::SetTileYields( const std::vector< std::pair< std::string, size_t > >& yields ) {
	if ( m_pages.yields ) {
		const bool need_to_add = m_texts.yields.empty();
		if ( !need_to_add ) {
			ASSERT_NOLOG( m_texts.yields.size() == yields.size(), "yields lines count mismatch" );
		}
		size_t label_top = 2;
		for ( size_t i = 0 ; i < yields.size() ; i++ ) {
			const auto& y = yields.at( i );
			const auto text = y.first + ": " + std::to_string( y.second );
			::ui::object::Label* label;
			if ( need_to_add ) {
				NEW( label, ::ui::object::Label, "BBTilePreviewTextLine" );
			}
			else {
				label = m_texts.yields.at( i );
			}
			label->SetText( text );
			if ( need_to_add ) {
				label->SetTop( label_top );
				m_texts.yields.push_back( label );
				m_pages.yields->AddChild( label );
				label_top += label->GetHeight() * 2; // TODO: 'with farm:' etc
			}
		}
	}
}

void TilePreview::HideYields() {
	ASSERT_NOLOG( m_pages.yields, "yields page not initialized" );
	for ( auto& label : m_texts.yields ) {
		m_pages.yields->RemoveChild( label );
	}
	m_texts.yields.clear();
}

}
}
}
