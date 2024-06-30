#include "TileObject.h"

#include "tile/Tile.h"

namespace task {
namespace game {

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
