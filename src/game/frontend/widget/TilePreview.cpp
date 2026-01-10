#include "TilePreview.h"

#include "ui/UI.h"
#include "ui/dom/Widget.h"
#include "game/frontend/tile/Tile.h"
#include "types/mesh/Render.h"
#include "ui/geometry/Rectangle.h"
#include "scene/actor/Mesh.h"
#include "game/frontend/Game.h"

namespace game {
namespace frontend {
namespace widget {

TilePreview::TilePreview( const Game* const game, ui::UI* const ui )
	: Widget( game, ui, ui::WT_TILE_PREVIEW, "tile-preview" ) {}

void TilePreview::Register( ui::dom::Widget* const widget ) {

}

void TilePreview::Update( const tile::Tile* const tile ) {
	WITH_WIDGET( this, &tile ) {
		widget->Clear();

		auto* g = widget->GetGeometry()->AsRectangle();
		const auto& render = tile->GetRenderData();

		size_t index = 0;
		for ( auto& preview_mesh : render.preview_meshes ) {
			NEWV( mesh_copy, types::mesh::Render, *preview_mesh );
			g->AddMesh( mesh_copy );
			NEWV( actor, scene::actor::Mesh, "UI::Widget::TilePreview::Layer" + std::to_string( index ), mesh_copy );
			g->AddActor( actor );
			actor->SetTexture( m_game->GetTerrainTexture() );
			widget->SetActor( actor, index++ );
		}

	} );
}

}
}
}
