#include "LockTiles.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/map/tile/TileManager.h"
#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

LockTiles::LockTiles(
	const size_t initiator_slot,
	const map::tile::positions_t& tile_positions,
	const size_t lock_owner_slot
)
	: TileLocksEvent( initiator_slot, ET_LOCK_TILES, "LockTiles", tile_positions )
	, m_lock_owner_slot( lock_owner_slot ) {
	//
}

const std::string* LockTiles::Validate( GSE_CALLABLE, Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to lock tiles" );
	}
	return ValidateTilePositions( game, false );
}

gse::Value* const LockTiles::Apply( GSE_CALLABLE, Game* game ) const {
	game->GetTM()->LockTiles( m_lock_owner_slot, m_tile_positions );
	return VALUEEXT( gse::value::Undefined, game->GetGCSpace() );
}

void LockTiles::Serialize( types::Buffer& buf, const LockTiles* event ) {
	buf.WriteInt( event->m_lock_owner_slot );
	TileLocksEvent::Serialize< LockTiles >( buf, event );
}

LockTiles* LockTiles::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const auto lock_owner_slot = buf.ReadInt();
	return TileLocksEvent::Unserialize< LockTiles >( buf, initiator_slot, lock_owner_slot );
}

}
}
}
