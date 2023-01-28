#include "Tiles.h"

#include <cstring>

namespace game {
namespace world {
namespace map {

Tiles::Tiles( const size_t width, const size_t height )
	: m_width( width )
	, m_height( height )
{
	size_t sz = sizeof( Tile ) * width * height;
	m_data = (Tile*)malloc( sz );
	memset( ptr( m_data, 0, sz ), 0, sz );
	
	sz = sizeof( Tile::elevation_t ) * ( width/* / 2*/ ) * 2;
	m_top_vertex_row = (Tile::elevation_t*)malloc( sz );
	memset( ptr( m_top_vertex_row, 0, sz ), 0, sz );

	sz = sizeof( Tile::elevation_t ) * ( width/* / 2*/ );
	m_top_right_vertex_row = (Tile::elevation_t*)malloc( sz );
	memset( ptr( m_top_right_vertex_row, 0, sz ), 0, sz );

	Tile* tile;
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = At( x, y );
			
			// link to other tiles
			tile->W = ( x >= 2 ) ? At( x - 2, y ) : At( m_width - 1 - ( 1 - ( y % 2 ) ), y );
			tile->NW = ( y >= 1 )
				? ( ( x >= 1 )
					? At( x - 1, y - 1 )
					: At( m_width - 1, y - 1 )
				)
				: tile
			;
			tile->N = ( y >= 2 ) ? At( x, y - 2 ) : tile;
			tile->NE = ( y >= 1 )
				? ( ( x < m_width - 1 )
					? At( x + 1, y - 1 )
					: At( 0, y - 1 )
				)
				: tile
			;
			tile->E = ( x < m_width - 2 ) ? At( x + 2, y ) : At( y % 2, y );
			tile->SE = ( y < m_height - 1 )
				? ( ( x < m_width - 1 )
					? At( x + 1, y + 1 )
					: At( 0, y + 1 )
				)
				: tile
			;
			tile->S = ( y < m_height - 2 ) ? At( x, y + 2 ) : tile;
			tile->SW = ( y < m_height - 1 )
				? ( ( x >= 1 )
					? At( x - 1, y + 1 )
					: At( m_width - 1, y + 1 )
				)
				: tile
			;
			
			if ( y >= 2 ) {
				tile->elevation.top = &tile->N->elevation.bottom;
			}
			else if ( y > 0 ) {
				ASSERT( x > 0, "x is zero while y isn't" );
				tile->elevation.top = At( x - 1, y - 1)->elevation.right;
			}
			else {
				tile->elevation.top = TopVertexAt( x/* / 2*/, y );
			}
			
			if ( x >= 2 ) {
				tile->elevation.left = tile->W->elevation.right;
			}
			
			if ( y == 0 ) {
				tile->elevation.right = TopRightVertexAt( x/* / 2*/ );
			}
			else if ( x < m_width - 1 ) {
				tile->elevation.right = &At( x + 1, y - 1 )->elevation.bottom;
			}
		}
		
		// link left edge to right edge and vice versa
		if ( y % 2 ) {
			At( m_width - 1, y )->elevation.right = ( y > 0 ? &At( 0, y - 1 )->elevation.bottom : TopRightVertexAt( 0 ) );
		}
		At( y % 2, y )->elevation.left = At( m_width - 1 - ( 1 - ( y % 2 ) ), y )->elevation.right;
	}

#ifdef DEBUG
	// check that all pointers are linked to something
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = At( x, y );
			
#define CHECKTILE( _what ) ASSERT( tile->_what, "tile " #_what " not linked at " + to_string( x ) + "x" + to_string( y ) );
			
			CHECKTILE( W );
			CHECKTILE( NW );
			CHECKTILE( N );
			CHECKTILE( NE );
			CHECKTILE( E );
			CHECKTILE( SE );
			CHECKTILE( S );
			CHECKTILE( SW );
			
			CHECKTILE( elevation.left );
			CHECKTILE( elevation.top );
			CHECKTILE( elevation.right );
			
#undef CHECKTILE
		}
	}
#endif
	
}

Tiles::~Tiles() {
	free( m_data );
	free( m_top_vertex_row );
	free( m_top_right_vertex_row );
}

const size_t Tiles::GetWidth() const {
	return m_width;
}

const size_t Tiles::GetHeight() const {
	return m_height;
}
	
Tile* Tiles::At( const size_t x, const size_t y ) const {
	ASSERT( x < m_width, "invalid x tile coordinate" );
	ASSERT( y < m_height, "invalid y tile coordinate" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile coordinate axis oddity differs" );
	return (Tile*)( m_data + m_width * y + x / 2 );
}

Tile::elevation_t* Tiles::TopVertexAt( const size_t x, const size_t y ) const {
	ASSERT( x < m_width, "invalid top vertex x coordinate" );
	ASSERT( y < 2, "invalid top vertex y coordinate" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "topvertexat tile coordinate axis oddity differs" );
	return (Tile::elevation_t*)( m_top_vertex_row + m_width * y + x );
}

Tile::elevation_t* Tiles::TopRightVertexAt( const size_t x ) const {
	ASSERT( x < m_width, "invalid top right vertex x coordinate" );
	return (Tile::elevation_t*)( m_top_right_vertex_row + x );
}

void Tiles::Validate() {
	if ( !m_is_validated ) {
		Log( "Validating map" );
		
		Tile* tile;
		for ( auto y = 0 ; y < m_height ; y++ ) {
			for ( auto x = 0 ; x < m_width ; x++ ) {
				if ( ( y % 2 ) != ( x % 2 ) ) {
					continue;
				}
				tile = At( x, y );
				
				if ( tile->moisture > 3 ) {
					Log( "tile moisture overflow ( " + to_string( tile->moisture ) + " > 3 ) at " + to_string( x ) + "x" + to_string( y ) );
				}
				if ( tile->rockyness > 3 ) {
					Log( "tile rockyness overflow ( " + to_string( tile->rockyness ) + " > 3 ) at " + to_string( x ) + "x" + to_string( y ) );
				}
				
			}
		}
		
		m_is_validated = true;
	}
}

}
}
}
