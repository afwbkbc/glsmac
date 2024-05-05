#include "LockTiles.h"

#include "game/Game.h"
#include "game/State.h"

namespace game {
namespace event {

LockTiles::LockTiles(
	const size_t initiator_slot,
	const map::Tile::tile_positions_t& tile_positions,
	const size_t lock_owner_slot
)
	: TileLocksEvent( initiator_slot, ET_LOCK_TILES, "LockTiles", tile_positions )
	, m_lock_owner_slot( lock_owner_slot ) {
	//
}

const std::string* LockTiles::Validate( Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to lock tiles" );
	}
	return ValidateTilePositions( game, false );
}

const gse::Value LockTiles::Apply( game::Game* game ) const {
	game->LockTiles( m_lock_owner_slot, m_tile_positions );
	return VALUE( gse::type::Undefined );
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
