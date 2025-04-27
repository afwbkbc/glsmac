#pragma once

#include "game/backend/map/tile/Types.h"
#include "util/Timer.h"

namespace game {
namespace backend {

class Game;

namespace map {
namespace tile {

class Tile;

class TileLock {
public:
	static const size_t MAX_WAIT_MS;

	typedef std::vector< Tile* > tiles_t;

	TileLock( const tiles_t& tile_positions );
	const bool Matches( const tiles_t& tile_positions ) const;
	const bool IsExpired();

	TileLock& operator=( const TileLock& other );

private:
	tiles_t m_tile_positions = {};
	util::Timer m_expiration_timer;
};

}
}
}
}
