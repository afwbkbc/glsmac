#include "TilePreview.h"

#include "ui/UI.h"
#include "ui/dom/Widget.h"
#include "game/frontend/tile/Tile.h"
#include "types/mesh/Render.h"
#include "ui/geometry/Rectangle.h"
#include "scene/actor/Mesh.h"
#include "game/frontend/Game.h"
#include "game/backend/map/tile/Tile.h"
#include "game/frontend/tile/TileManager.h"
#include "game/frontend/sprite/InstancedSpriteManager.h"
#include "scene/actor/Instanced.h"
#include "scene/actor/Sprite.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/Player.h"

namespace game {
namespace frontend {
namespace widget {

TilePreview::TilePreview( Game* const game, ui::UI* const ui )
	: Widget(
	game, ui, ui::WT_TILE_PREVIEW, "tile-preview", {
		{ "tile", { true, gse::VT_OBJECT, ::game::backend::map::tile::Tile::WRAP_CLASS } },
	}
) {}

void TilePreview::Register( ui::dom::Widget* const widget ) {
	widget->OnUpdate(
		F_WIDGET_UPDATE( this ) {
			if ( data ) {
				auto* const t = ::game::backend::map::tile::Tile::Unwrap( data->value.at( "tile" ) );
				ASSERT( t, "invalid tile ptr" );
				const auto* const tile = m_game->GetTM()->GetTile( t->coord.x, t->coord.y );
				ASSERT( tile, "invalid tile" );
				Update( widget, tile );
			}
		}
	);
}

void TilePreview::Update( ui::dom::Widget* const widget, const void* const data ) {
	widget->Clear();

	const auto* tile = (const tile::Tile*)data;

	const auto& render = tile->GetRenderData();

	size_t index = 0;

	// add tile texture
	auto* const texture = m_game->GetTerrainTexture();
	types::Color tint = { 1.5f, 1.5f, 1.5f, 1.0f }; // to compensate for lack of lighting
	for ( auto& preview_mesh : render.preview_meshes ) {
		auto* const actor = AddMeshAndTexture( widget, index++, preview_mesh, texture );
		actor->SetTintColor( tint );
	}

	// add tile sprites
	for ( auto& s : render.sprites ) {
		auto* actor = m_game->GetISM()->GetInstancedSpriteByKey( s )->actor;
		auto sprite = actor->GetSpriteActor();
		auto mesh = sprite->GenerateMesh();
		AddMeshAndTexture( widget, index++, mesh, sprite->GetTexture() );
	}

}

}
}
}
