#include <cstring>
#include <random>

#include "Tiles.h"

#include "util/Clamper.h"
#include "util/random/Random.h"

namespace game {
namespace backend {
namespace map {
namespace tile {

Tiles::Tiles( const uint32_t width, const uint32_t height ) {
	if ( width || height ) {
		Resize( width, height );
	}
}

Tiles::~Tiles() {
}

void Tiles::Resize( const uint32_t width, const uint32_t height ) {
	ASSERT( width > 0, "can't resize to zero width" );
	ASSERT( height > 0, "can't resize to zero height" );
	ASSERT( !( width & 1 ), "can't resize to non-even width" );
	ASSERT( !( height & 1 ), "can't resize to non-even height" );

	if ( width != m_width || height != m_height ) {
		Log( "Initializing tiles ( " + std::to_string( width ) + " x " + std::to_string( height ) + " )" );

		m_width = width;
		m_height = height;

		m_data.resize( width * height );
		m_top_vertex_row.resize( m_width * 2 );
		m_top_right_vertex_row.resize( width );

		Tile* tile;
		for ( auto y = 0 ; y < m_height ; y++ ) {
			for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
				tile = &At( x, y );

				// link to other tiles
				tile->W = ( x >= 2 )
					? &At( x - 2, y )
					: &At( m_width - 1 - ( 1 - ( y % 2 ) ), y );
				tile->NW = ( y >= 1 )
					? ( ( x >= 1 )
						? &At( x - 1, y - 1 )
						: &At( m_width - 1, y - 1 )
					)
					: tile;
				tile->N = ( y >= 2 )
					? &At( x, y - 2 )
					: tile;
				tile->NE = ( y >= 1 )
					? ( ( x < m_width - 1 )
						? &At( x + 1, y - 1 )
						: &At( 0, y - 1 )
					)
					: tile;
				tile->E = ( x < m_width - 2 )
					? &At( x + 2, y )
					: &At( y % 2, y );
				tile->SE = ( y < m_height - 1 )
					? ( ( x < m_width - 1 )
						? &At( x + 1, y + 1 )
						: &At( 0, y + 1 )
					)
					: tile;
				tile->S = ( y < m_height - 2 )
					? &At( x, y + 2 )
					: tile;
				tile->SW = ( y < m_height - 1 )
					? ( ( x >= 1 )
						? &At( x - 1, y + 1 )
						: &At( m_width - 1, y + 1 )
					)
					: tile;

				tile->elevation.bottom = &tile->elevation_data.bottom;
				tile->elevation.center = &tile->elevation_data.center;

				if ( y >= 2 ) {
					tile->elevation.top = tile->N->elevation.bottom;
				}
				else if ( y > 0 ) {
					ASSERT( x > 0, "x is zero while y isn't" );
					tile->elevation.top = AtConst( x - 1, y - 1 ).elevation.right;
				}
				else {
					tile->elevation.top = TopVertexAt( x, y );
				}

				if ( x >= 2 ) {
					tile->elevation.left = tile->W->elevation.right;
				}

				if ( y == 0 ) {
					tile->elevation.right = TopRightVertexAt( x );
				}
				else if ( x < m_width - 1 ) {
					tile->elevation.right = AtConst( x + 1, y - 1 ).elevation.bottom;
				}
			}

			// link left edge to right edge and vice versa
			if ( y % 2 ) {
				At( m_width - 1, y ).elevation.right = ( y > 0
					? AtConst( 0, y - 1 ).elevation.bottom
					: TopRightVertexAt( 0 )
				);
			}
			At( y % 2, y ).elevation.left = AtConst( m_width - 1 - ( 1 - ( y % 2 ) ), y ).elevation.right;

		}

		// add some state variables
		for ( auto y = 0 ; y < m_height ; y++ ) {
			for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
				tile = &At( x, y );

#ifdef DEBUG
				// check that all pointers are linked to something
#define CHECKTILE( _what ) ASSERT( tile->_what, "tile " #_what " not linked at " + std::to_string( x ) + "x" + std::to_string( y ) );

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
#endif

				tile->coord.x = x;
				tile->coord.y = y;

				// generate pointer vectors
				tile->elevation.corners = {
					tile->elevation.left,
					tile->elevation.top,
					tile->elevation.right,
					tile->elevation.bottom,
				};
				tile->neighbours = {
					tile->W,
					tile->NW,
					tile->N,
					tile->NE,
					tile->E,
					tile->SE,
					tile->S,
					tile->SW,
				};
			}
		}
	}
}

void Tiles::Clear() {
	ASSERT( !m_data.empty(), "tiles not initialized" );

	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			At( x, y ).Clear();
		}
	}
}

const uint32_t Tiles::GetWidth() const {
	return m_width;
}

const uint32_t Tiles::GetHeight() const {
	return m_height;
}

Tile& Tiles::At( const size_t x, const size_t y ) {
	ASSERT( x < m_width, "invalid x tile coordinate ( " + std::to_string( x ) + " >= " + std::to_string( m_width ) + " )" );
	ASSERT( y < m_height, "invalid y tile coordinate ( " + std::to_string( y ) + " >= " + std::to_string( m_height ) + " )" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile coordinate axis oddity differs" );
	return m_data.at( y * m_width + x / 2 );
}

const Tile& Tiles::AtConst( const size_t x, const size_t y ) const {
	ASSERT( x < m_width, "invalid x tile coordinate ( " + std::to_string( x ) + " >= " + std::to_string( m_width ) + " )" );
	ASSERT( y < m_height, "invalid y tile coordinate ( " + std::to_string( y ) + " >= " + std::to_string( m_height ) + " )" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile coordinate axis oddity differs" );
	return m_data.at( y * m_width + x / 2 );
}

std::vector< Tile >* Tiles::GetTilesPtr() {
	return &m_data;
}

elevation_t* Tiles::TopVertexAt( const size_t x, const size_t y ) {
	ASSERT( x < m_width, "invalid top vertex x coordinate" );
	ASSERT( y < 2, "invalid top vertex y coordinate" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "topvertexat tile coordinate axis oddity differs" );
	return &m_top_vertex_row.at( m_width * y + x );
}

elevation_t* Tiles::TopRightVertexAt( const size_t x ) {
	ASSERT( x < m_width, "invalid top right vertex x coordinate" );
	return &m_top_right_vertex_row.at( x );
}

void Tiles::Validate( MT_CANCELABLE ) {
	if ( !m_is_validated ) {
		Log( "Validating map" );

		const Tile* tile;
		for ( auto y = 0 ; y < m_height ; y++ ) {
			for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
				tile = &AtConst( x, y );

				if ( tile->moisture > MOISTURE_RAINY ) {
					Log( "tile moisture overflow ( " + std::to_string( tile->moisture ) + " > 3 ) at " + std::to_string( x ) + "x" + std::to_string( y ) );
				}
				if ( tile->rockiness > ROCKINESS_ROCKY ) {
					Log( "tile rockiness overflow ( " + std::to_string( tile->rockiness ) + " > 3 ) at " + std::to_string( x ) + "x" + std::to_string( y ) );
				}

				MT_RETIF();
			}
		}

		m_is_validated = true;
	}
}

const size_t Tiles::GetDataCount() const {
	return m_data.size();
}

void Tiles::FixTopBottomRows( util::random::Random* random ) {
	// set top and bottom rows to zero elevation, otherwise they can't be displayed correctly
	Tile* tile;
	elevation_t top_bottom_elevation;
	for ( auto x = 0 ; x < m_width ; x++ ) {
		top_bottom_elevation = random->GetBool()
			? 1
			: -1;
		if ( x % 2 == 0 ) {
			tile = &At( x, 0 );
			*tile->elevation.left = *tile->elevation.right = top_bottom_elevation;
			*tile->elevation.top = 0;
		}
		else {
			tile = &At( x, GetHeight() - 1 );
			*tile->elevation.left = *tile->elevation.right = top_bottom_elevation;
			*tile->elevation.bottom = 0;
		}
	}

	for ( auto y = 0 ; y < m_height ; y++ ) {
		if ( y > 1 || y < m_height - 2 ) {
			continue;
		}
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			tile = &At( x, y );

			tile->Update();
		}
	}
}

const std::vector< Tile* > Tiles::GetVector( MT_CANCELABLE ) {
	std::vector< Tile* > tiles = {};
	const size_t tiles_count = GetDataCount() / 2; // / 2 because SMAC coordinate system
	tiles.reserve( tiles_count );
	for ( size_t y = 0 ; y < m_height ; y++ ) {
		for ( size_t x = y & 1 ; x < m_width ; x += 2 ) {
			tiles.push_back( &At( x, y ) );
			MT_RETIFV( {} );
		}
	}
	ASSERT( tiles.size() == tiles_count, "tiles count mismatch on load ( " + std::to_string( tiles.size() ) + " != " + std::to_string( tiles_count ) + " )" );

	return tiles;
}

const types::Buffer Tiles::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( m_width );
	buf.WriteInt( m_height );

	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			buf.WriteString( AtConst( x, y ).Serialize().ToString() );
		}
	}

	buf.WriteBool( m_is_validated );

	return buf;
}

void Tiles::Unserialize( types::Buffer buf ) {

	size_t width = buf.ReadInt();
	size_t height = buf.ReadInt();

	m_width = m_height = 0;
	Resize( width, height );

	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			At( x, y ).Unserialize( types::Buffer( buf.ReadString() ) );
		}
	}

	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			At( x, y ).Update();
		}
	}

}

}
}
}
}
