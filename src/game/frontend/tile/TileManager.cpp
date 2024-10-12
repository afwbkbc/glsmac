#include "TileManager.h"

#include "game/backend/map/Consts.h"
#include "util/FS.h"

namespace game {
namespace frontend {
namespace tile {

TileManager::TileManager( Game* game )
	: m_game( game ) {
	//
}

void TileManager::InitTiles( const types::Vec2< size_t > map_size ) {
	ASSERT( m_tiles.empty(), "tile states already set" );

	m_map_size = map_size;
	m_tiles.reserve( m_map_size.x * m_map_size.y / 2 ); // / 2 because smac coordinate system
	for ( size_t y = 0 ; y < m_map_size.y ; y++ ) {
		for ( size_t x = y & 1 ; x < m_map_size.x ; x += 2 ) {
			m_tiles.insert(
				{
					y * m_map_size.x + x,
					Tile{
						{
							x,
							y
						}
					}
				}
			);
		}
	}

	// link tiles to neighbours
	for ( size_t y = 0 ; y < m_map_size.y ; y++ ) {
		for ( size_t x = y & 1 ; x < m_map_size.x ; x += 2 ) {
			auto* ts = GetTile(
				{
					x,
					y
				}
			);

			ts->W = ( x >= 2 )
				? GetTile( x - 2, y )
				: GetTile( m_map_size.x - 1 - ( 1 - ( y % 2 ) ), y );
			ts->NW = ( y >= 1 )
				? ( ( x >= 1 )
					? GetTile( x - 1, y - 1 )
					: GetTile( m_map_size.x - 1, y - 1 )
				)
				: ts;
			ts->N = ( y >= 2 )
				? GetTile( x, y - 2 )
				: ts;
			ts->NE = ( y >= 1 )
				? ( ( x < m_map_size.x - 1 )
					? GetTile( x + 1, y - 1 )
					: GetTile( 0, y - 1 )
				)
				: ts;
			ts->E = ( x < m_map_size.x - 2 )
				? GetTile( x + 2, y )
				: GetTile( y % 2, y );
			ts->SE = ( y < m_map_size.y - 1 )
				? ( ( x < m_map_size.x - 1 )
					? GetTile( x + 1, y + 1 )
					: GetTile( 0, y + 1 )
				)
				: ts;
			ts->S = ( y < m_map_size.y - 2 )
				? GetTile( x, y + 2 )
				: ts;
			ts->SW = ( y < m_map_size.y - 1 )
				? ( ( x >= 1 )
					? GetTile( x - 1, y + 1 )
					: GetTile( m_map_size.x - 1, y + 1 )
				)
				: ts;
		}
	}
}

Tile* TileManager::GetTile( const size_t x, const size_t y ) {
	ASSERT( !m_tiles.empty(), "tiles not initialized" );
	ASSERT( x < m_map_size.x, "tile x overflow" );
	ASSERT( y < m_map_size.y, "tile y overflow" );
	return &m_tiles.at( y * m_map_size.x + x );
}

Tile* TileManager::GetTile( const types::Vec2< size_t >& coords ) {
	return GetTile( coords.x, coords.y );
}

Tile* TileManager::GetSelectedTile() const {
	return m_selected_tile;
}

void TileManager::SelectTile( Tile* tile ) {
	m_selected_tile = tile;
}

void TileManager::DeselectTile() {
	m_previously_deselected_tile = m_selected_tile;
	if ( m_selected_tile ) {
		m_selected_tile->Render();
		m_selected_tile = nullptr;
	}
}

Tile* TileManager::GetPreviouslyDeselectedTile() const {
	return m_previously_deselected_tile;
}

}
}
}
