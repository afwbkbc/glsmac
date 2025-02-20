#include "RequestTileUnlocks.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/map/tile/TileManager.h"
#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

RequestTileUnlocks::RequestTileUnlocks(
	const size_t initiator_slot,
	const map::tile::positions_t& tile_positions,
	const size_t nothing
)
	: TileLocksEvent( initiator_slot, ET_REQUEST_TILE_UNLOCKS, "RequestTileUnlocks", tile_positions ) {
	//
}

const std::string* RequestTileUnlocks::Validate( Game* game ) const {
	return ValidateTilePositions( game, true, m_initiator_slot );
}

gse::Value* const RequestTileUnlocks::Apply( Game* game ) const {
	game->GetTM()->RequestTileUnlocks( m_initiator_slot, m_tile_positions );
	return VALUEEXT( gse::value::Undefined, game->GetGCSpace() );
}

void RequestTileUnlocks::Serialize( types::Buffer& buf, const RequestTileUnlocks* event ) {
	TileLocksEvent::Serialize< RequestTileUnlocks >( buf, event );
}

RequestTileUnlocks* RequestTileUnlocks::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	return TileLocksEvent::Unserialize< RequestTileUnlocks >( buf, initiator_slot, 0 );
}

}
}
}
