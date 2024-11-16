#include "TileLock.h"

#include "game/backend/Game.h"

namespace game {
namespace backend {
namespace map {
namespace tile {

const size_t TileLock::MAX_WAIT_MS = 10000;

TileLock::TileLock( const map::tile::positions_t& tile_positions )
	: m_tile_positions( tile_positions ) {
	m_expiration_timer.SetTimeout( MAX_WAIT_MS );
}

const bool TileLock::Matches( const map::tile::positions_t& tile_positions ) const {
	return m_tile_positions == tile_positions;
}

const bool TileLock::IsExpired() {
	return m_expiration_timer.HasTicked();
}

TileLock& TileLock::operator=( const TileLock& other ) {
	m_tile_positions = other.m_tile_positions;
	m_expiration_timer = other.m_expiration_timer;
	return *this;
}

}
}
}
}
