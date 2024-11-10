#pragma once

#include "common/Common.h"

#include "gse/Wrappable.h"
#include "gse/type/Object.h"

#include "Types.h"
#include "TileLock.h"

namespace game {
namespace backend {

class Game;

namespace map {
namespace tile {

CLASS2( TileManager, common::Class, gse::Wrappable )
public:
	TileManager( Game* game );
	~TileManager();

	typedef std::function< void() > cb_oncomplete;

	void Clear();

	void SendTileLockRequest( const map::tile::positions_t& tile_positions, const cb_oncomplete& on_complete );
	void RequestTileLocks( const size_t initiator_slot, const map::tile::positions_t& tile_positions );
	void LockTiles( const size_t initiator_slot, const map::tile::positions_t& tile_positions );

	void SendTileUnlockRequest( const map::tile::positions_t& tile_positions );
	void RequestTileUnlocks( const size_t initiator_slot, const map::tile::positions_t& tile_positions );
	void UnlockTiles( const size_t initiator_slot, const map::tile::positions_t& tile_positions );

	void ProcessTileLockRequests();
	void ReleaseTileLocks( const size_t initiator_slot );

	WRAPDEFS_PTR( TileManager )

private:
	Game* m_game = nullptr;

	// server-side lock tracking
	struct tile_lock_request_t {
		const bool is_lock; // lock or unlock
		const size_t initiator_slot;
		const map::tile::positions_t tile_positions;
	};
	typedef std::vector< tile_lock_request_t > tile_lock_requests_t;  // requests fifo
	tile_lock_requests_t m_tile_lock_requests = {};
	std::unordered_map< size_t, std::vector< TileLock > > m_tile_locks = {}; // slot id, locks
	std::vector< std::pair< map::tile::positions_t, cb_oncomplete > > m_tile_lock_callbacks = {}; // tile positions (for matching), callback

	void AddTileLockRequest( const bool is_lock, const size_t initiator_slot, const map::tile::positions_t& tile_positions );

};

}
}
}
}
