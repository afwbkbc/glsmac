#include "RequestTileLocks.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/map/tile/TileManager.h"
#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

RequestTileLocks::RequestTileLocks(
	const size_t initiator_slot,
	const map::tile::positions_t& tile_positions,
	const size_t nothing
)
	: TileLocksEvent( initiator_slot, ET_REQUEST_TILE_LOCKS, "RequestTileLocks", tile_positions ) {
	//
}

const std::string* RequestTileLocks::Validate( GSE_CALLABLE, Game* game ) const {
	return ValidateTilePositions( game, false );
}

gse::Value* const RequestTileLocks::Apply( GSE_CALLABLE, Game* game ) const {
	game->GetTM()->RequestTileLocks( m_initiator_slot, m_tile_positions );
	return VALUEEXT( gse::value::Undefined, game->GetGCSpace() );
}

void RequestTileLocks::Serialize( types::Buffer& buf, const RequestTileLocks* event ) {
	TileLocksEvent::Serialize< RequestTileLocks >( buf, event );
}

RequestTileLocks* RequestTileLocks::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	return TileLocksEvent::Unserialize< RequestTileLocks >( buf, initiator_slot, 0 );
}

}
}
}
