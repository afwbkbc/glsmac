#include "TileManager.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/map/Map.h"
#include "game/backend/event/LockTiles.h"
#include "game/backend/event/UnlockTiles.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/value/Array.h"
#include "gse/ExecutionPointer.h"

namespace game {
namespace backend {
namespace map {
namespace tile {

TileManager::TileManager( Game* game )
	: m_game( game ) {
	//
}

TileManager::~TileManager() {
	//
}

void TileManager::Clear() {
	m_tile_lock_requests.clear();
	m_tile_locks.clear();
	m_tile_lock_callbacks.clear();
}

void TileManager::SendTileLockRequest( const map::tile::positions_t& tile_positions, const cb_oncomplete& on_complete ) {
	// testing
	/*m_tile_lock_callbacks.push_back(
		{
			tile_positions,
			on_complete
		}
	);*/
	LockTiles( m_game->GetSlotNum(), tile_positions );
	on_complete();
	//AddEvent( new event::RequestTileLocks( m_slot_num, tile_positions ) );
}

void TileManager::RequestTileLocks( const size_t initiator_slot, const map::tile::positions_t& tile_positions ) {
	if ( m_game->GetState()->IsMaster() ) { // huh?
		Log( "Tile locks request from " + std::to_string( initiator_slot ) + ": " + map::tile::Tile::TilePositionsToString( tile_positions, "" ) );
		AddTileLockRequest( true, initiator_slot, tile_positions );
	}
}

void TileManager::LockTiles( const size_t initiator_slot, const map::tile::positions_t& tile_positions ) {
	for ( const auto& pos : tile_positions ) {
		auto* tile = m_game->GetMap()->GetTile( pos.x, pos.y );
		ASSERT_NOLOG( !tile->IsLocked(), "tile " + pos.ToString() + " is already locked" );
		tile->Lock( initiator_slot );
	}
	for ( auto it = m_tile_lock_callbacks.begin() ; it != m_tile_lock_callbacks.end() ; it++ ) {
		const auto& it_positions = it->first;
		const auto sz = it_positions.size();
		if ( sz == tile_positions.size() ) {
			bool match = true;
			for ( size_t i = 0 ; i < sz ; i++ ) {
				if ( tile_positions.at( i ) != it_positions.at( i ) ) {
					match = false;
					break;
				}
			}
			if ( match ) {
				const auto cb = it->second;
				m_tile_lock_callbacks.erase( it );
				cb();
				break;
			}
		}
	}
}

void TileManager::SendTileUnlockRequest( const map::tile::positions_t& tile_positions ) {
	// testing
	UnlockTiles( m_game->GetSlotNum(), tile_positions );
	//AddEvent( new event::RequestTileUnlocks( m_slot_num, tile_positions ) );
}

void TileManager::RequestTileUnlocks( const size_t initiator_slot, const map::tile::positions_t& tile_positions ) {
	if ( m_game->GetState()->IsMaster() ) {
		Log( "Tile unlocks request from " + std::to_string( initiator_slot ) + ": " + map::tile::Tile::TilePositionsToString( tile_positions, "" ) );
		AddTileLockRequest( false, initiator_slot, tile_positions );
	}
}

void TileManager::UnlockTiles( const size_t initiator_slot, const map::tile::positions_t& tile_positions ) {
	for ( const auto& pos : tile_positions ) {
		auto* tile = m_game->GetMap()->GetTile( pos.x, pos.y );
		ASSERT_NOLOG( tile->IsLockedBy( initiator_slot ), "tile " + pos.ToString() + " is not locked by " + std::to_string( initiator_slot ) );
		tile->Unlock();
	}
}

void TileManager::ProcessTileLockRequests() {
	if ( m_game->IsRunning() ) {
		auto* state = m_game->GetState();
		if ( state->IsMaster() && !m_tile_lock_requests.empty() ) {
			const auto tile_lock_requests = m_tile_lock_requests;
			m_tile_lock_requests.clear();
			const auto slot_num = m_game->GetSlotNum();
			if ( !tile_lock_requests.empty() ) {
				m_game->GetState()->WithGSE(
					[ this, &tile_lock_requests, &state, &slot_num ]( GSE_CALLABLE ) {
						for ( const auto& req : tile_lock_requests ) {
							const auto slot_state = state->m_slots->GetSlot( req.initiator_slot ).GetState();
							if ( slot_state != slot::Slot::SS_PLAYER ) {
								// player disconnected?
								Log( "Skipping tile locks/unlocks for slot " + std::to_string( req.initiator_slot ) + " (invalid slot state: " + std::to_string( slot_state ) + ")" );
								continue;
							}
							auto it = m_tile_locks.find( req.initiator_slot );
							if ( it == m_tile_locks.end() ) {
								it = m_tile_locks.insert(
									{
										req.initiator_slot,
										{}
									}
								).first;
							}
							auto& locks = it->second;
							if ( req.is_lock ) {
								// lock
								Log( "Locking tiles for " + std::to_string( req.initiator_slot ) + ": " + map::tile::Tile::TilePositionsToString( req.tile_positions ) );
								locks.push_back( TileLock{ req.tile_positions } );
								auto e = new event::LockTiles( slot_num, req.tile_positions, req.initiator_slot );
								e->SetDestinationSlot( req.initiator_slot );
								m_game->AddEvent( GSE_CALL, e );
							}
							else {
								// unlock
								bool found = false;
								for ( auto locks_it = locks.begin() ; locks_it != locks.end() ; locks_it++ ) {
									if ( locks_it->Matches( req.tile_positions ) ) {
										found = true;
										Log( "Unlocking tiles for " + std::to_string( req.initiator_slot ) + ": " + map::tile::Tile::TilePositionsToString( req.tile_positions ) );
										locks.erase( locks_it );
										if ( locks.empty() ) {
											m_tile_locks.erase( it );
										}
										auto e = new event::UnlockTiles( slot_num, req.tile_positions, req.initiator_slot );
										e->SetDestinationSlot( req.initiator_slot );
										m_game->AddEvent( GSE_CALL, e );
										break;
									}
								}
								if ( !found ) {
									Log( "Could not find matching tile locks for " + std::to_string( req.initiator_slot ) + ": " + map::tile::Tile::TilePositionsToString( req.tile_positions ) );
								}
							}
						}
					}
				);
			}
		}
	}
}

void TileManager::ReleaseTileLocks( const size_t initiator_slot ) {
	const auto& it = m_tile_locks.find( initiator_slot );
	if ( it != m_tile_locks.end() ) {
		Log( "Releasing " + std::to_string( it->second.size() ) + " tile locks" );
		m_tile_locks.erase( it );
	}
}

WRAPIMPL_BEGIN( TileManager )
	WRAPIMPL_PROPS
		{
			"get_map_width",
			NATIVE_CALL( this ) {
				const auto* m = m_game->GetMap();
				return VALUE( gse::value::Int,, m->GetWidth() );
			} )
		},
		{
			"get_map_height",
			NATIVE_CALL( this ) {
				const auto* m = m_game->GetMap();
				return VALUE( gse::value::Int,, m->GetHeight() );
			})
		},
		{
			"get_tile",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( x, 0, Int );
				N_GETVALUE( y, 1, Int );
				const auto* m = m_game->GetMap();
				const auto w = m->GetWidth();
				const auto h = m->GetHeight();
				if ( x >= w ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "X coordinate exceeds map width ( " + std::to_string( x ) + " >= " + std::to_string( w ) + " )" );
				}
				if ( y >= h ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Y coordinate exceeds map height ( " + std::to_string( y ) + " >= " + std::to_string( h ) + " )" );
				}
				if ( x < 0 ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "X coordinate can't be negative ( " + std::to_string( x ) + " < 0" );
				}
				if ( y < 0 ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Y coordinate can't be negative ( " + std::to_string( y ) + " < 0" );
				}
				if ( x % 2 != y % 2 ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "X and Y oddity differs ( " + std::to_string( x ) + " % 2 != " + std::to_string( y ) + " % 2 )" );
				}
				return m->GetTile( x, y )->Wrap( GSE_CALL, gc_space );
			} )
		},
		{
			"lock_tiles",
				NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( tiles, 0, Array );
				N_GET_CALLABLE( on_complete, 1 );
				map::tile::positions_t tile_positions = {};
				tile_positions.reserve( tiles.size() );
				for ( const auto& tileobj : tiles ) {
					N_UNWRAP( tile, tileobj, map::tile::Tile );
					tile_positions.push_back( tile->coord );
				}
				SendTileLockRequest( tile_positions, [ this, gc_space, on_complete, tile_positions, ctx, si, ep ]() {
					auto ep2 = ep;
					on_complete->Run( gc_space, ctx, si, ep2, {
						VALUE( gse::callable::Native,, ctx, [ this, tile_positions ](
							GSE_CALLABLE,
							const gse::value::function_arguments_t& arguments
						)  {
							SendTileUnlockRequest( tile_positions );
							return VALUE( gse::value::Undefined );
						} )
					});
				});
				return VALUE( gse::value::Undefined );
			})
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( TileManager )

void TileManager::AddTileLockRequest( const bool is_lock, const size_t initiator_slot, const map::tile::positions_t& tile_positions ) {
	ASSERT_NOLOG( m_game->GetState() && m_game->GetState()->IsMaster(), "only master can manage tile locks" );
	m_tile_lock_requests.push_back(
		{
			is_lock,
			initiator_slot,
			tile_positions
		}
	);
}

}
}
}
}
