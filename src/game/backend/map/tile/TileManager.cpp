#include "TileManager.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/map/Map.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/value/Array.h"
#include "gse/ExecutionPointer.h"

namespace game {
namespace backend {
namespace map {
namespace tile {

TileManager::TileManager( Game* game )
	: gse::GCWrappable( game->GetGCSpace() )
	, m_game( game ) {
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

void TileManager::LockTiles( const size_t initiator_slot, const tiles_t& tiles ) {
	for ( const auto& tile : tiles ) {
		ASSERT( !tile->IsLocked(), "tile " + tile->coord.ToString() + " is already locked" );
		tile->Lock( initiator_slot );
	}
	for ( auto it = m_tile_lock_callbacks.begin() ; it != m_tile_lock_callbacks.end() ; it++ ) {
		const auto& it_positions = it->first;
		const auto sz = it_positions.size();
		if ( sz == tiles.size() ) {
			bool match = true;
			for ( const auto& tile : tiles ) {
				if ( it_positions.find( tile ) == it_positions.end() ) {
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

const Tile* TileManager::FindLockedTile( const tiles_t& tiles ) {
	for ( const auto& tile : tiles ) {
		if ( tile->IsLocked() ) {
			return tile;
		}
	}
	return nullptr;
}

void TileManager::UnlockTiles( const size_t initiator_slot, const tiles_t& tiles ) {
	for ( const auto& tile : tiles ) {
		ASSERT( tile->IsLockedBy( initiator_slot ), "tile " + tile->coord.ToString() + " is not locked by " + std::to_string( initiator_slot ) );
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
					this,
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
								Log( "Locking tiles for " + std::to_string( req.initiator_slot ) + ": " + TilesToString( req.tile_positions ) );
								locks.push_back( TileLock{ req.tile_positions } );
							}
							else {
								// unlock
								bool found = false;
								for ( auto locks_it = locks.begin() ; locks_it != locks.end() ; locks_it++ ) {
									if ( locks_it->Matches( req.tile_positions ) ) {
										found = true;
										Log( "Unlocking tiles for " + std::to_string( req.initiator_slot ) + ": " + TilesToString( req.tile_positions ) );
										locks.erase( locks_it );
										if ( locks.empty() ) {
											m_tile_locks.erase( it );
										}
										break;
									}
								}
								if ( !found ) {
									Log( "Could not find matching tile locks for " + std::to_string( req.initiator_slot ) + ": " + TilesToString( req.tile_positions ) );
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
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( TileManager )

const std::string TileManager::TilesToString( const tiles_t& tiles, std::string prefx ) {
	// TODO: refactor
	std::string result = TS_ARR_BEGIN( "Tiles" );
	for ( const auto& tile : tiles ) {
		std::string prefix = prefx + TS_OFFSET;
		result += TS_OFFSET + TS_OBJ_BEGIN( "Tile" ) +
			TS_OBJ_PROP_NUM( "x", tile->coord.x ) +
			TS_OBJ_PROP_NUM( "y", tile->coord.y ) +
			TS_OBJ_END() + ",\n";
	}
	std::string prefix = prefx;
	result += TS_ARR_END();
	return result;
}



}
}
}
}
