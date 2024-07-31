#include "TileObject.h"

#include "game/frontend/tile/Tile.h"

namespace game {
namespace frontend {

TileObject::TileObject( const tile_object_type_t type, tile::Tile* tile )
	: m_type( type )
	, m_tile( tile ) {
	//
}

void TileObject::UpdateFromTile() {
	ASSERT_NOLOG( m_tile, "tile not set" );
	SetRenderCoords( m_tile->GetRenderData().coords.InvertY() );
}

const TileObject::tile_object_type_t TileObject::GetType() const {
	return m_type;
}

}
}
