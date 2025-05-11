#pragma once

#include <unordered_set>

#include "common/Common.h"

#include "gse/GCWrappable.h"
#include "gse/value/Object.h"

#include "Types.h"
#include "TileLock.h"

namespace game {
namespace backend {

class Game;

namespace map {
namespace tile {

class Tile;

CLASS2( TileManager, common::Class, gse::GCWrappable )
public:
	TileManager( Game* game );
	~TileManager();

	typedef std::function< void() > cb_oncomplete;

	void Clear();

	typedef std::unordered_set< map::tile::Tile* > tiles_t;

	const Tile* FindLockedTile( const tiles_t& tiles );
	void LockTiles( const size_t initiator_slot, const tiles_t& tiles );
	void UnlockTiles( const size_t initiator_slot, const tiles_t& tiles );

	void ProcessTileLockRequests();
	void ReleaseTileLocks( const size_t initiator_slot );

	WRAPDEFS_PTR( TileManager )

private:
	Game* m_game = nullptr;

	// server-side lock tracking
	struct tile_lock_request_t {
		const bool is_lock; // lock or unlock
		const size_t initiator_slot;
		const tiles_t tile_positions;
	};
	typedef std::vector< tile_lock_request_t > tile_lock_requests_t;  // requests fifo
	tile_lock_requests_t m_tile_lock_requests = {};
	std::unordered_map< size_t, std::vector< TileLock > > m_tile_locks = {}; // slot id, locks
	std::vector< std::pair< tiles_t, cb_oncomplete > > m_tile_lock_callbacks = {}; // tile positions (for matching), callback

	const std::string TilesToString( const tiles_t& tiles, std::string prefx = "" );
};

}
}
}
}
