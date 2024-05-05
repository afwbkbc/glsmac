#pragma once

#include "map/Tile.h"
#include "util/Timer.h"

namespace game {
class Game;

class TileLock {
public:
	static const size_t MAX_WAIT_MS;

	TileLock( const map::Tile::tile_positions_t& tile_positions );
	const bool Matches( const map::Tile::tile_positions_t& tile_positions ) const;
	const bool IsExpired();

	TileLock& operator=( const TileLock& other );

private:
	map::Tile::tile_positions_t m_tile_positions;
	util::Timer m_expiration_timer;
};

}
