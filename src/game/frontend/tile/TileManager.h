#pragma once

#include "common/Common.h"

#include <unordered_map>
#include <cstddef>

#include "Tile.h"
#include "types/Vec2.h"
#include "util/Clamper.h"

namespace game {
namespace frontend {

class Game;

namespace tile {

CLASS( TileManager, ::common::Class )

	TileManager( Game* game );

	void InitTiles( const types::Vec2< size_t > map_size );

	Tile* GetTile( const size_t x, const size_t y );
	Tile* GetTile( const types::Vec2< size_t >& coords );

	Tile* GetSelectedTile() const;
	void SelectTile( Tile* tile );
	void DeselectTile();
	Tile* GetPreviouslyDeselectedTile() const;

private:

	Game* m_game;

	types::Vec2< size_t > m_map_size = {};
	std::unordered_map< size_t, Tile > m_tiles = {};

	Tile* m_selected_tile = nullptr;
	Tile* m_previously_deselected_tile = nullptr;

};

}
}
}
