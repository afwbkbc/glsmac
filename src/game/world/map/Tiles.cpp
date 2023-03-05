#include <cstring>
#include <algorithm>
#include <random>

#include "Tiles.h"

#include "util/Clamper.h"
#include "Map.h"

namespace game {
namespace world {
namespace map {

Tiles::Tiles( const uint32_t width, const uint32_t height, Random* random )
	: m_random( random )
{
	Resize( width, height );
}

Tiles::~Tiles() {
	free( m_data );
	free( m_top_vertex_row );
	free( m_top_right_vertex_row );
}

void Tiles::Resize( const uint32_t width, const uint32_t height ) {
	
	if ( width != m_width || height != m_height ) {
		Log( "Initializing tiles ( " + std::to_string( width ) + " x " + std::to_string( height ) + " )" );
		
		m_width = width;
		m_height = height;
		
		// warning: allocating objects without new (because it's faster), check this if any memory problems arise
		if ( m_data ) {
			free( m_data );
		}
		size_t sz = sizeof( Tile ) * width * height;
		m_data = (Tile*)malloc( sz );
		memset( ptr( m_data, 0, sz ), 0, sz );

		if ( m_top_vertex_row ) {
			free( m_top_vertex_row );
		}
		sz = sizeof( Tile::elevation_t ) * ( width ) * 2;
		m_top_vertex_row = (Tile::elevation_t*)malloc( sz );
		memset( ptr( m_top_vertex_row, 0, sz ), 0, sz );

		if ( m_top_right_vertex_row ) {
			free( m_top_right_vertex_row );
		}
		sz = sizeof( Tile::elevation_t ) * ( width );
		m_top_right_vertex_row = (Tile::elevation_t*)malloc( sz );
		memset( ptr( m_top_right_vertex_row, 0, sz ), 0, sz );

		Tile* tile;
		for ( auto y = 0 ; y < m_height ; y++ ) {
			for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
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

				tile->elevation.bottom = &tile->elevation_data.bottom;
				tile->elevation.center = &tile->elevation_data.center;

				if ( y >= 2 ) {
					tile->elevation.top = tile->N->elevation.bottom;
				}
				else if ( y > 0 ) {
					ASSERT( x > 0, "x is zero while y isn't" );
					tile->elevation.top = At( x - 1, y - 1)->elevation.right;
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
					tile->elevation.right = At( x + 1, y - 1 )->elevation.bottom;
				}
			}

			// link left edge to right edge and vice versa
			if ( y % 2 ) {
				At( m_width - 1, y )->elevation.right = ( y > 0 ? At( 0, y - 1 )->elevation.bottom : TopRightVertexAt( 0 ) );
			}
			At( y % 2, y )->elevation.left = At( m_width - 1 - ( 1 - ( y % 2 ) ), y )->elevation.right;

		}

		// add some state variables
		for ( auto y = 0 ; y < m_height ; y++ ) {
			for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
				tile = At( x, y );

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

const uint32_t Tiles::GetWidth() const {
	return m_width;
}

const uint32_t Tiles::GetHeight() const {
	return m_height;
}
	
Tile* Tiles::At( const size_t x, const size_t y ) const {
	ASSERT( x < m_width, "invalid x tile coordinate" );
	ASSERT( y < m_height, "invalid y tile coordinate" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile coordinate axis oddity differs" );
	return (Tile*)( m_data + y * m_width + x / 2 );
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
		//Log( "Validating map" );
		
		Tile* tile;
		for ( auto y = 0 ; y < m_height ; y++ ) {
			for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
				tile = At( x, y );
				
				if ( tile->moisture > 3 ) {
					Log( "tile moisture overflow ( " + std::to_string( tile->moisture ) + " > 3 ) at " + std::to_string( x ) + "x" + std::to_string( y ) );
				}
				if ( tile->rockyness > 3 ) {
					Log( "tile rockyness overflow ( " + std::to_string( tile->rockyness ) + " > 3 ) at " + std::to_string( x ) + "x" + std::to_string( y ) );
				}
				
			}
		}
		
		m_is_validated = true;
	}
}

void Tiles::Finalize() {
	
	FixExtremeSlopes(); // in case map generator didn't call it
	NormalizeElevationRange();
	
	float desired_land_amount = 0.5;
	float acceptable_inaccuracy = 0.01f; // will increase every try to prevent infinite loop
	
	// TODO: fix it
	//do {
		//SetLandAmount( 0.5f );
		//NormalizeElevationRange();
		FixTopBottomRows();
		RemoveExtremeSlopes( Map::s_consts.tile.maximum_allowed_slope_elevation );
		acceptable_inaccuracy *= 1.1f;
	//} while ( fabs( GetLandAmount() - desired_land_amount ) > acceptable_inaccuracy );
	
	Log( "Final land amount: " + std::to_string( GetLandAmount() ) );
}

void Tiles::SetLandAmount( const float amount ) {
	
	Log( "Setting land amount to " + std::to_string( amount ) );
	
	float acceptable_inaccuracy = 0.01f;
	Tile::elevation_t elevation = Tile::ELEVATION_LEVEL_COAST; // start with 0
	Tile::elevation_t elevation_change_by = ( Tile::ELEVATION_MAX - Tile::ELEVATION_MIN ) / 2 ; // for bisecting to find best elevation difference for desired land amount
	float current_land_amount;
	int8_t last_direction = 0;
	while ( elevation_change_by > 1 ) {
		current_land_amount = GetLandAmount( elevation );
		Log( "ELEVATION=" + std::to_string( elevation ) + " LAND_AMOUNT=" + std::to_string( current_land_amount ) );
		
		if ( current_land_amount >= amount - acceptable_inaccuracy && current_land_amount <= amount + acceptable_inaccuracy ) {
			// found optimal elevation
			break;
		}
		
		if ( current_land_amount < amount ) {
			elevation += elevation_change_by;
			if ( last_direction != 1 ) {
				last_direction = 1;
				elevation_change_by /= 2;
			}
		}
		else {
			elevation -= elevation_change_by;
			if ( last_direction != -1 ) {
				last_direction = -1;
				elevation_change_by /= 2;
			}
		}
	}
	
	RaiseAllTilesBy( elevation );
}

const float Tiles::GetLandAmount( Tile::elevation_t elevation_diff ) {
	size_t land_tiles = 0;
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			if ( *At( x, y )->elevation.center > -elevation_diff ) {
				land_tiles++;
			}
		}
	}
	return (float) land_tiles / ( ( m_width * m_height ) / 2 ); // divide by 2 because of SMAC coordinate system (not every tile exists)
}

void Tiles::RaiseAllTilesBy( Tile::elevation_t amount ) {
	Log( "Raising all tiles by " + std::to_string( amount ) );
	Tile* tile;
	// process in random order
	std::vector< Tile* > tiles;
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			tiles.push_back( At( x, y ) );
		}
	}
	std::mt19937 g( m_random->GetUInt( 0, UINT32_MAX - 1 ) );
	std::shuffle( tiles.begin(), tiles.end(), g );
	
	for ( auto& tile : tiles ) {
		*tile->elevation.center += amount;
		*tile->elevation.bottom += amount;
	}
	
	for ( auto y = 0; y < 2 ; y++ ) {
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( y == 0 ) {
				*( m_top_right_vertex_row + x ) += amount;
			}
			*( m_top_vertex_row + y * m_width + x ) += amount;
		}
	}
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			At( x, y )->Update();
		}
	}
}

const std::pair< Tile::elevation_t, Tile::elevation_t > Tiles::GetElevationsRange() const {
	std::pair< Tile::elevation_t, Tile::elevation_t > result = { 0, 0 };
	Tile* tile;
	// determine min and max elevations from generated tiles
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			tile = At( x, y );
			for ( auto& c : tile->elevation.corners ) {
				if ( *c < result.first ) {
					result.first = *c;
				}
				else if ( *c > result.second ) {
					result.second = *c;
				}
			}
		}
	}
	Log( "Elevations range: min=" + std::to_string( result.first ) + " max=" + std::to_string( result.second ) );
	return result;
}

void Tiles::FixExtremeSlopes() {
	auto elevations_range = GetElevationsRange();
	util::Clamper<Tile::elevation_t> converter( Tile::ELEVATION_MIN, Tile::ELEVATION_MAX, elevations_range.first, elevations_range.second );
	RemoveExtremeSlopes( abs( converter.Clamp( Map::s_consts.tile.maximum_allowed_slope_elevation ) ) );
}

Random* Tiles::GetRandom() const {
	return m_random;
}

void Tiles::NormalizeElevationRange() {
	auto elevations_range = GetElevationsRange();
	util::Clamper<Tile::elevation_t> converter( elevations_range.first, elevations_range.second, Tile::ELEVATION_MIN, Tile::ELEVATION_MAX );
	
	// process in random order
	std::vector< Tile* > tiles;
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			tiles.push_back( At( x, y ) );
		}
	}
	std::mt19937 g( m_random->GetUInt( 0, UINT32_MAX - 1 ) );
	std::shuffle( tiles.begin(), tiles.end(), g );
	
	for ( auto& tile : tiles ) {
		tile->elevation_data.bottom = converter.Clamp( tile->elevation_data.bottom );
		tile->elevation_data.center = converter.Clamp( tile->elevation_data.center );
	}

	// convert top rows too
	for ( auto y = 0; y < 2 ; y++ ) {
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( y == 0 ) {
				*( m_top_right_vertex_row + x ) = converter.Clamp(*( m_top_right_vertex_row + x ));
			}
			*( m_top_vertex_row + y * m_width + x ) = converter.Clamp(*( m_top_vertex_row + y * m_width + x ));
		}
	}
}

void Tiles::RemoveExtremeSlopes( const Tile::elevation_t max_allowed_diff ) {
	Tile::elevation_t elevation_fixby_change = 1;
	Tile::elevation_t elevation_fixby_max = max_allowed_diff / 3; // to prevent infinite loops when it grows so large it starts creating new extreme slopes
	float elevation_fixby_div_change = 0.001f; // needed to prevent infinite loops when nearby tiles keep 'fixing' each other forever
	size_t pass = 0;
	Tile* tile;
	Tile::elevation_t elevation_fixby = 0;
	float elevation_fixby_div = 1.0f;
	bool found = true;
	size_t i;

	// process in random order
	std::vector< Tile* > tiles;
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			tiles.push_back( At( x, y ) );
		}
	}

	while ( found ) {
		if ( elevation_fixby < elevation_fixby_max ) {
			elevation_fixby += elevation_fixby_change;
		}
		elevation_fixby_div += elevation_fixby_div_change;
		Log( "Checking/fixing extreme slopes (pass " + std::to_string( ++pass ) + ")" );
		found = false;
		
		std::mt19937 g( m_random->GetUInt( 0, UINT32_MAX - 1 ) );
		std::shuffle( tiles.begin(), tiles.end(), g );
	
		// don't run in normal cycle because it can give terrain some straight edges, go in random order instead
		// assume that on average we'll hit all tiles (but skipping some is no big deal)
		for ( i = 0 ; i < tiles.size() ; i++ ) {
			tile = tiles[ i ];

			#define x( _a, _b ) \
				if ( abs( *tile->elevation._a - *tile->elevation._b ) > max_allowed_diff ) { \
					/*Log( "fixing slope: " + std::to_string( *tile->elevation._a ) + "," + std::to_string( *tile->elevation._b ) + " / " + std::to_string( elevation_fixby ) );*/ \
					*tile->elevation._a += ( *tile->elevation._a < *tile->elevation._b ) ? elevation_fixby : -elevation_fixby; \
					*tile->elevation._b += ( *tile->elevation._b < *tile->elevation._a ) ? elevation_fixby : -elevation_fixby; \
					*tile->elevation._a /= elevation_fixby_div; \
					*tile->elevation._b /= elevation_fixby_div; \
					found = true; \
				}
				x( left, right );
				x( left, top );
				x( left, bottom );
				x( right, top );
				x( right, bottom );
				x( top, bottom );
			#undef x

			if ( found ) {
				tile->Update();
			}
		}
	}
}

void Tiles::FixTopBottomRows() {
	// set top and bottom rows to zero elevation, otherwise they can't be displayed correctly
	Tile* tile;
	Tile::elevation_t top_bottom_elevation;
	for ( auto x = 0 ; x < m_width ; x++ ) {
		top_bottom_elevation = m_random->GetBool() ? 1 : -1;
		if ( x % 2 == 0 ) {
			tile = At( x, 0 );
			*tile->elevation.left = *tile->elevation.right = top_bottom_elevation;
			*tile->elevation.top = 0;
		}
		else {
			tile = At( x, GetHeight() - 1 );
			*tile->elevation.left = *tile->elevation.right = top_bottom_elevation;
			*tile->elevation.bottom = 0;
		}
	}
	
	for ( auto y = 0 ; y < m_height ; y++ ) {
		if ( y > 1 || y < m_height - 2 ) {
			continue;
		}
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			tile = At( x, y );
			
			tile->Update();
		}
	}
}

const Buffer Tiles::Serialize() const {
	Buffer buf;

	buf.WriteInt( m_width );
	buf.WriteInt( m_height );
	
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			buf.WriteString( At( x, y )->Serialize().ToString() );
		}
	}
	
	return buf;
}

void Tiles::Unserialize( Buffer buf ) {
	
	size_t width = buf.ReadInt();
	size_t height = buf.ReadInt();
	
	Resize( width, height );
	
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			At( x, y )->Unserialize( Buffer( buf.ReadString() ) );
		}
	}
	
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = y & 1 ; x < m_width ; x += 2 ) {
			At( x, y )->Update();
		}
	}
	
	Validate();
}

}
}
}
