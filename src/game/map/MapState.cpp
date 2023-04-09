#include "MapState.h"

namespace game {
namespace map {

MapState::~MapState() {
	if ( m_tile_states ) {
		for ( size_t y = 0 ; y < dimensions.y ; y++ ) {
			for ( size_t x = y & 1 ; x < dimensions.x ; x+= 2 ) {

				auto* ts = At( x, y );

				if ( ts->moisture_original ) {
					DELETE( ts->moisture_original );
					ts->moisture_original = nullptr;
				}
				if ( ts->river_original ) {
					DELETE( ts->river_original );
					ts->river_original = nullptr;
				}
				ts->sprites.clear();
/*				for ( auto& a : ts->sprites ) {
					a.actor->RemoveInstance( a.instance );
				}*/
			}
		}
		free( m_tile_states );
	}
}

TileState* MapState::At( const size_t x, const size_t y ) const {
	ASSERT( x < dimensions.x, "tile state x overflow" );
	ASSERT( y < dimensions.y, "tile state y overflow" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile state axis oddity differs" );
	return &m_tile_states[ y * dimensions.x + x / 2 ];
}

void MapState::LinkTileStates( MT_CANCELABLE ) {
	
	ASSERT( !m_tile_states, "m_tile_states already set" );
	{
		size_t sz = sizeof( TileState ) * dimensions.y * dimensions.x;
		m_tile_states = (TileState*)malloc( sz );
		memset( ptr( m_tile_states, 0, sz ), 0, sz );
	}

	Log( "Linking tile states" );
	
	// link to each other via pointers
	// TODO: refactor this and Tiles
	for ( auto y = 0 ; y < dimensions.y ; y++ ) {
		for ( auto x = y & 1 ; x < dimensions.x ; x += 2 ) {
			auto* ts = At( x, y );
			
			ts->W = ( x >= 2 ) ? At( x - 2, y ) : At( dimensions.x - 1 - ( 1 - ( y % 2 ) ), y );
			ts->NW = ( y >= 1 )
				? ( ( x >= 1 )
					? At( x - 1, y - 1 )
					: At( dimensions.x - 1, y - 1 )
				)
				: ts
			;
			ts->N = ( y >= 2 ) ? At( x, y - 2 ) : ts;
			ts->NE = ( y >= 1 )
				? ( ( x < dimensions.x - 1 )
					? At( x + 1, y - 1 )
					: At( 0, y - 1 )
				)
				: ts
			;
			ts->E = ( x < dimensions.x - 2 ) ? At( x + 2, y ) : At( y % 2, y );
			ts->SE = ( y < dimensions.y - 1 )
				? ( ( x < dimensions.x - 1 )
					? At( x + 1, y + 1 )
					: At( 0, y + 1 )
				)
				: ts
			;
			ts->S = ( y < dimensions.y - 2 ) ? At( x, y + 2 ) : ts;
			ts->SW = ( y < dimensions.y - 1 )
				? ( ( x >= 1 )
					? At( x - 1, y + 1 )
					: At( dimensions.x - 1, y + 1 )
				)
				: ts
			;

			MT_RETIF();
		}
	}
}

TileState* MapState::GetTileState( const size_t x, const size_t y ) const {
	ASSERT( x < dimensions.x, "tile state x overflow" );
	ASSERT( y < dimensions.y, "tile state y overflow" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile state axis oddity differs" );
	return &m_tile_states[ y * dimensions.x + x / 2 ];
}

}
}
