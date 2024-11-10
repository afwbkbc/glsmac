#include "UnlockTiles.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/map/tile/TileManager.h"
#include "gse/type/Undefined.h"

namespace game {
namespace backend {
namespace event {

UnlockTiles::UnlockTiles(
	const size_t initiator_slot,
	const map::tile::positions_t& tile_positions,
	const size_t lock_owner_slot
)
	: TileLocksEvent( initiator_slot, ET_UNLOCK_TILES, "UnlockTiles", tile_positions )
	, m_lock_owner_slot( lock_owner_slot ) {
	//
}

const std::string* UnlockTiles::Validate( Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to unlock tiles" );
	}
	return ValidateTilePositions( game, true, m_lock_owner_slot );
}

const gse::Value UnlockTiles::Apply( Game* game ) const {
	game->GetTM()->UnlockTiles( m_lock_owner_slot, m_tile_positions );
	return VALUE( gse::type::Undefined );
}

void UnlockTiles::Serialize( types::Buffer& buf, const UnlockTiles* event ) {
	buf.WriteInt( event->m_lock_owner_slot );
	TileLocksEvent::Serialize< UnlockTiles >( buf, event );
}

UnlockTiles* UnlockTiles::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const auto lock_owner_slot = buf.ReadInt();
	return TileLocksEvent::Unserialize< UnlockTiles >( buf, initiator_slot, lock_owner_slot );
}

}
}
}
