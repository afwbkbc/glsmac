#include "Resource.h"

#include "ui_legacy/object/Mesh.h"
#include "scene/actor/Instanced.h"
#include "scene/actor/Sprite.h"
#include "types/mesh/Simple.h"
#include "types/mesh/Render.h"
#include "types/texture/Texture.h"
#include "game/frontend/base/Base.h"
#include "game/frontend/tile/Tile.h"
#include "game/frontend/Game.h"
#include "game/frontend/sprite/InstancedSpriteManager.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {
namespace base_popup {
namespace center_area {

Resource::Resource( CenterArea* center_area )
	: CenterAreaPage( center_area ) {
	m_resource_tiles.reserve( 21 );
}

void Resource::Create() {
	CenterAreaPage::Create();
}

void Resource::Destroy() {
	HideTilePreviews();

	CenterAreaPage::Destroy();
}

void Resource::Update( base::Base* base ) {
	HideTilePreviews();
	UpdateResourceTiles( base );
	for ( const auto& it : m_resource_tiles ) {
		PreviewTile( it.first, it.second );
	}
	for ( const auto& it : m_resource_tiles ) {
		PreviewTileSprites( it.first, it.second );
	}
}

void Resource::UpdateResourceTiles( base::Base* base ) {
	m_resource_tiles.clear();
	const auto* const t = base->GetTile();
	const auto mh = m_game->GetMapHeight();
#define X( _t, _x, _y ) m_resource_tiles.insert( { _t, { _x, _y } } )
	// from top to bottom
	if ( t->m_coords.y > 2 ) {
		X( t->N->NW, -1, -3 );
		X( t->N->NE, 1, -3 );
	}
	if ( t->m_coords.y > 1 ) {
		X( t->N, 0, -2 );
		X( t->N->W, -2, -2 );
		X( t->N->E, 2, -2 );
	}
	if ( t->m_coords.y > 0 ) {
		X( t->NW, -1, -1 );
		X( t->NW->W, -3, -1 );
		X( t->NE, 1, -1 );
		X( t->NE->E, 3, -1 );
	}
	X( t, 0, 0 );
	X( t->E, 2, 0 );
	X( t->W, -2, 0 );
	if ( t->m_coords.y < mh - 1 ) {
		X( t->SW, -1, 1 );
		X( t->SW->W, -3, 1 );
		X( t->SE, 1, 1 );
		X( t->SE->E, 3, 1 );
		if ( t->m_coords.y < mh - 2 ) {
			X( t->S, 0, 2 );
			X( t->S->W, -2, 2 );
			X( t->S->E, 2, 2 );
			if ( t->m_coords.y < mh - 3 ) {
				X( t->S->SW, -1, 3 );
				X( t->S->SE, 1, 3 );
			}
		}
	}
#undef X
}

void Resource::PreviewTile( const tile::Tile* tile, const types::Vec2< int8_t > pos ) {
	tile_preview_t tile_preview = {};

	const auto left = 144 + 48 * pos.x;
	const auto top = 72 + 24 * pos.y;

	const auto& render = tile->GetRenderData();
	for ( auto& mesh : render.preview_meshes ) {
		NEWV( preview, ::ui_legacy::object::Mesh, "BPCenterAreaResourceTile" );
		NEWV( mesh_copy, types::mesh::Render, *mesh );
		preview->SetMesh( mesh_copy );
		preview->SetTexture( m_game->GetTerrainTexture() );
		preview->SetLeft( left );
		preview->SetTop( top );
		tile_preview.push_back( preview );
		AddChild( preview );
	}
	m_tile_previews.push_back( tile_preview );
}

void Resource::PreviewTileSprites( const tile::Tile* tile, const types::Vec2< int8_t > pos ) {
	tile_preview_t tile_preview = {};

	const auto left = 144 + 48 * pos.x;
	const auto top = 72 + 24 * pos.y;

	const auto& render = tile->GetRenderData();

	// copy sprites from tile
	for ( auto& s : render.sprites ) {
		auto* actor = m_game->GetISM()->GetInstancedSpriteByKey( s )->actor;
		auto sprite = actor->GetSpriteActor();
		NEWV( sprite_preview, ::ui_legacy::object::Mesh, "BPCenterAreaResourceSprite" );
		sprite_preview->SetMesh( sprite->GenerateMesh() );
		sprite_preview->SetTintColor(
			{
				0.7f,
				0.7f,
				0.7f,
				1.0f
			}
		); // lower brightness a bit because it's too high otherwise for some reason
		sprite_preview->SetTexture( sprite->GetTexture() );
		sprite_preview->SetLeft( left );
		sprite_preview->SetTop( top );
		tile_preview.push_back( sprite_preview );
		AddChild( sprite_preview );
	}

	const auto* base = tile->GetBase();
	if ( base ) {
		const auto& base_render = base->GetRenderData();
		NEWV( preview, ::ui_legacy::object::Mesh, "BPCenterAreaResourceSprite" );
		preview->SetMesh( base_render.base.mesh->CopyAsRenderMesh() );
		preview->SetTintColor(
			{
				0.7f,
				0.7f,
				0.7f,
				1.0f
			}
		);
		preview->SetTexture( base_render.base.texture );
		preview->SetLeft( left );
		preview->SetTop( top - 10 ); // TODO: why?
		tile_preview.push_back( preview );
		AddChild( preview );
	}

	m_tile_previews.push_back( tile_preview );
}

void Resource::HideTilePreviews() {
	for ( auto& tile_preview : m_tile_previews ) {
		for ( auto& preview_mesh : tile_preview ) {
			RemoveChild( preview_mesh );
		}
	}
	m_tile_previews.clear();
}

}
}
}
}
}
}
