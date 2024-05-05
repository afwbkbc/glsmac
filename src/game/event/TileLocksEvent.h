#pragma once

#include "Event.h"

#include "game/map/Tile.h"

namespace game {
namespace event {

class RequestTileLocks;

class LockTiles;

class TileLocksEvent : public Event {
public:
	TileLocksEvent(
		const size_t initiator_slot,
		const event_type_t event_type,
		const std::string event_name,
		const map::Tile::tile_positions_t& tile_positions
	);

	TS_DEF()

private:
	friend class RequestTileLocks;
	friend class LockTiles;
	friend class RequestTileUnlocks;
	friend class UnlockTiles;

	template< class T >
	static void Serialize( types::Buffer& buf, const T* event );
	template< class T >
	static T* Unserialize( types::Buffer& buf, const size_t initiator_slot, const size_t lock_owner_slot );

protected:
	const map::Tile::tile_positions_t m_tile_positions;
	const std::string* ValidateTilePositions( const Game* game, const bool should_be_locked, const size_t should_be_locked_by = 0 ) const;

private:
	const std::string m_event_name;
};

}
}
