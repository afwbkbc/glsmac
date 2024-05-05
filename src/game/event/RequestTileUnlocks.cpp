#include "RequestTileUnlocks.h"

#include "game/Game.h"
#include "game/State.h"

namespace game {
namespace event {

RequestTileUnlocks::RequestTileUnlocks(
	const size_t initiator_slot,
	const map::Tile::tile_positions_t& tile_positions,
	const size_t nothing
)
	: TileLocksEvent( initiator_slot, ET_REQUEST_TILE_UNLOCKS, "RequestTileUnlocks", tile_positions ) {
	//
}

const std::string* RequestTileUnlocks::Validate( Game* game ) const {
	return ValidateTilePositions( game, true, m_initiator_slot );
}

const gse::Value RequestTileUnlocks::Apply( game::Game* game ) const {
	game->RequestTileUnlocks( m_initiator_slot, m_tile_positions );
	return VALUE( gse::type::Undefined );
}

void RequestTileUnlocks::Serialize( types::Buffer& buf, const RequestTileUnlocks* event ) {
	TileLocksEvent::Serialize< RequestTileUnlocks >( buf, event );
}

RequestTileUnlocks* RequestTileUnlocks::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	return TileLocksEvent::Unserialize< RequestTileUnlocks >( buf, initiator_slot, 0 );
}

}
}
