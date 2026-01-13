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

namespace game {
namespace frontend {
namespace widget {

TilePreview::TilePreview( const Game* const game, ui::UI* const ui )
	: Widget(
	game, ui, ui::WT_TILE_PREVIEW, "tile-preview", {
		{ "tile", { gse::VT_OBJECT, ::game::backend::map::tile::Tile::WRAP_CLASS } },
	}
) {}

void TilePreview::Register( ui::dom::Widget* const widget ) {
	widget->OnUpdate(
		F_WIDGET_UPDATE( this ) {
			const auto* const t = ::game::backend::map::tile::Tile::Unwrap( data->value.at( "tile" ) );
			ASSERT( t, "invalid tile ptr" );
			const auto* const tile = m_game->GetTM()->GetTile( t->coord.x, t->coord.y );
			ASSERT( tile, "invalid tile" );
			Update( widget, tile );
		}
	);
}

void TilePreview::Update( ui::dom::Widget* const widget, const tile::Tile* const tile ) {
	widget->Clear();

	const auto& render = tile->GetRenderData();

	size_t index = 0;
	auto* const texture = m_game->GetTerrainTexture();
	for ( auto& preview_mesh : render.preview_meshes ) {
		AddMeshAndTexture( widget, index++, preview_mesh, texture );
	}
}

}
}
}
