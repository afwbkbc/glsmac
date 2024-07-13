#include "TileLocksEvent.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/map/Map.h"

#include "RequestTileLocks.h"
#include "LockTiles.h"
#include "RequestTileUnlocks.h"
#include "UnlockTiles.h"

namespace game {
namespace backend {
namespace event {

TileLocksEvent::TileLocksEvent(
	const size_t initiator_slot,
	const event_type_t event_type,
	const std::string event_name,
	const map::tile::positions_t& tile_positions
)
	: Event( initiator_slot, event_type )
	, m_event_name( event_name )
	, m_tile_positions( tile_positions ) {
	//
}

TS_BEGIN( TileLocksEvent )
		TS_FUNC_BEGIN( m_event_name ) +
			map::tile::Tile::TilePositionsToString( m_tile_positions, TS_PREFIX ) +
		TS_FUNC_END()
TS_END()

template< class T >
void TileLocksEvent::Serialize( types::Buffer& buf, const T* event ) {
	buf.WriteInt( event->m_tile_positions.size() );
	for ( const auto& pos : event->m_tile_positions ) {
		buf.WriteInt( pos.x );
		buf.WriteInt( pos.y );
	}
}

template< class T >
T* TileLocksEvent::Unserialize( types::Buffer& buf, const size_t initiator_slot, const size_t lock_owner_slot ) {
	const auto positions_count = buf.ReadInt();
	map::tile::positions_t positions = {};
	positions.reserve( positions_count );
	for ( size_t i = 0 ; i < positions_count ; i++ ) {
		const auto x = buf.ReadInt();
		const auto y = buf.ReadInt();
		positions.push_back(
			{
				(size_t)x,
				(size_t)y
			}
		);
	}
	return new T( initiator_slot, positions, lock_owner_slot );
}

const std::string* TileLocksEvent::ValidateTilePositions( const Game* game, const bool should_be_locked, const size_t should_be_locked_by ) const {
	const auto w = game->GetMap()->GetWidth();
	const auto h = game->GetMap()->GetHeight();
	for ( const auto& pos : m_tile_positions ) {
		if ( pos.x >= w ) {
			return Error( "Tile X position overflow ( " + std::to_string( pos.x ) + " >= " + std::to_string( w ) );
		}
		if ( pos.y > h ) {
			return Error( "Tile Y position overflow ( " + std::to_string( pos.y ) + " >= " + std::to_string( h ) );
		}
		const auto* tile = game->GetMap()->GetTile( pos.x, pos.y );
		if ( should_be_locked ) {
			if ( !tile->IsLocked() ) {
				return Error( "Tile " + pos.ToString() + " is not locked" );
			}
			if ( !tile->IsLockedBy( should_be_locked_by ) ) {
				return Error( "Tile " + pos.ToString() + " lock owner slot id mismatch" );
			}
		}
		else {
			if ( tile->IsLocked() ) {
				return Error( "Tile " + pos.ToString() + " is already locked" );
			}
		}
	}
	return Ok();
}

#define TEMPLATIZE( _class ) \
    template void TileLocksEvent::Serialize( types::Buffer& buf, const _class* event ); \
    template _class* TileLocksEvent::Unserialize( types::Buffer& buf, const size_t initiator_slot, const size_t lock_owner_slot );

TEMPLATIZE( RequestTileLocks );
TEMPLATIZE( LockTiles );
TEMPLATIZE( RequestTileUnlocks );
TEMPLATIZE( UnlockTiles );

#undef TEMPLATIZE

}
}
}
