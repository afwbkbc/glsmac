#include "TileObject.h"

#include "game/frontend/tile/Tile.h"

namespace game {
namespace frontend {

TileObject::TileObject( tile::Tile* tile )
	: m_tile( tile ) {
	//
}

void TileObject::UpdateFromTile() {
	ASSERT_NOLOG( m_tile, "tile not set" );
	SetRenderCoords( m_tile->GetRenderData().coords.InvertY() );
}

}
}
