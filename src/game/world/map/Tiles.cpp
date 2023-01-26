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
	
	sz = sizeof( Tile::elevation_t ) * width * 2;
	m_top_vertex_row = (Tile::elevation_t*)malloc( sz );
	memset( ptr( m_top_vertex_row, 0, sz ), 0, sz );

	sz = sizeof( Tile::elevation_t ) * width;
	m_top_right_vertex_row = (Tile::elevation_t*)malloc( sz );
	memset( ptr( m_top_right_vertex_row, 0, sz ), 0, sz );

	Tile* tile;
	// link vertexes together where needed
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = 0 ; x < m_width ; x++ ) {

			tile = At( x, y );
			
			if ( y >= 2 ) {
				tile->elevation.top = &(At( x, y - 2 )->elevation.bottom);
			}
			else if ( y > 0 ) {
				tile->elevation.top = At( x, y - 1 )->elevation.right;
			}
			else {
				tile->elevation.top = TopVertexAt( x, y );
			}
				
			if ( x > 0 ) {
				tile->elevation.left = At( x - 1, y )->elevation.right;
			}
			else {
				// link to other side of map later
			}
			
			if ( y == 0 ) {
				tile->elevation.right = TopRightVertexAt( x );
			}
			else if ( y % 2 == 1 && x < m_width - 1 ) {
				tile->elevation.right = &At( x + 1, y - 1 )->elevation.bottom;
			}
			else if ( y % 2 == 0 ) {
				tile->elevation.right = &At( x, y - 1 )->elevation.bottom;
			}
			
		}
		
		// link left edge to right edge and vice versa
		if ( y % 2 == 1 ) {
			At( m_width - 1, y )->elevation.right = ( y > 0 ? &At( 0, y - 1 )->elevation.bottom : TopRightVertexAt( 0 ) );
		}
		At( 0, y )->elevation.left = At( m_width - 1, y )->elevation.right;
	}
	
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
	return (Tile*)( m_data + m_width * y + x );
}

Tile::elevation_t* Tiles::TopVertexAt( const size_t x, const size_t y ) const {
	ASSERT( x < m_width, "invalid top vertex x coordinate" );
	ASSERT( y < 2, "invalid top vertex y coordinate" );
	return (Tile::elevation_t*)( m_top_vertex_row + m_width * y + x );
}

Tile::elevation_t* Tiles::TopRightVertexAt( const size_t x ) const {
	ASSERT( x < m_width, "invalid top right vertex x coordinate" );
	return (Tile::elevation_t*)( m_top_right_vertex_row + x );
}

}
}
}
