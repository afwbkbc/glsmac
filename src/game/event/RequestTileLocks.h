#pragma once

#include "TileLocksEvent.h"

#include "game/map/Tile.h"

namespace game {
namespace event {

class RequestTileLocks : public TileLocksEvent {
public:
	RequestTileLocks(
		const size_t initiator_slot,
		const map::Tile::tile_positions_t& tile_positions,
		const size_t nothing = 0
	);

	const std::string* Validate( Game* game ) const override;
	const gse::Value Apply( game::Game* game ) const override;

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const RequestTileLocks* event );
	static RequestTileLocks* Unserialize( types::Buffer& buf, const size_t initiator_slot );
};

}
}
