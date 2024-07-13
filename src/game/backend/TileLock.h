#pragma once

#include "game/backend/map/tile/Types.h"
#include "util/Timer.h"

namespace game {
namespace backend {

class Game;

class TileLock {
public:
	static const size_t MAX_WAIT_MS;

	TileLock( const map::tile::positions_t& tile_positions );
	const bool Matches( const map::tile::positions_t& tile_positions ) const;
	const bool IsExpired();

	TileLock& operator=( const TileLock& other );

private:
	map::tile::positions_t m_tile_positions;
	util::Timer m_expiration_timer;
};

}
}
