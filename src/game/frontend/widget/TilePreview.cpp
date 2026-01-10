#include "TilePreview.h"

#include "ui/UI.h"

namespace game {
namespace frontend {
namespace widget {

TilePreview::TilePreview( ui::UI* const ui )
	: Widget( ui, ui::WT_TILE_PREVIEW, "tile-preview" ) {}

void TilePreview::Register( ui::dom::Widget* const widget ) {
	Log( "TILE PREVIEW REGISTER" );
}

void TilePreview::Update( const tile::Tile* const tile ) {
	WITH_WIDGET( this ) {
		// TODO
	} );
}

}
}
}
