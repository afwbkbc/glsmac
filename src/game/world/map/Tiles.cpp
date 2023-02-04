#include "Tiles.h"

#include <cstring>

#include "util/Clamper.h"
#include "Map.h"

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
	
	sz = sizeof( Tile::elevation_t ) * ( width ) * 2;
	m_top_vertex_row = (Tile::elevation_t*)malloc( sz );
	memset( ptr( m_top_vertex_row, 0, sz ), 0, sz );

	sz = sizeof( Tile::elevation_t ) * ( width );
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
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = At( x, y );
			
#ifdef DEBUG
	// check that all pointers are linked to something
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
	
	Log( "Final land amount: " + to_string( GetLandAmount() ) );
}

void Tiles::SetLandAmount( const float amount ) {
	
	Log( "Setting land amount to " + to_string( amount ) );
	
	float acceptable_inaccuracy = 0.01f;
	Tile::elevation_t elevation = Tile::ELEVATION_LEVEL_COAST; // start with 0
	Tile::elevation_t elevation_change_by = ( Tile::ELEVATION_MAX - Tile::ELEVATION_MIN ) / 2 ; // for bisecting to find best elevation difference for desired land amount
	float current_land_amount;
	int8_t last_direction = 0;
	while ( elevation_change_by > 1 ) {
		current_land_amount = GetLandAmount( elevation );
		Log( "ELEVATION=" + to_string( elevation ) + " LAND_AMOUNT=" + to_string( current_land_amount ) );
		
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
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			if ( *At( x, y )->elevation.center > -elevation_diff ) {
				land_tiles++;
			}
		}
	}
	return (float) land_tiles / ( ( m_width * m_height ) / 2 ); // divide by 2 because of SMAC coordinate system (not every tile exists)
}

void Tiles::RaiseAllTilesBy( Tile::elevation_t amount ) {
	Log( "Raising all tiles by " + to_string( amount ) );
	Tile* tile;
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = At( x, y );
			*tile->elevation.center += amount;
			*tile->elevation.bottom += amount;
		}
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
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			At( x, y )->Update();
		}
	}
}

const pair< Tile::elevation_t, Tile::elevation_t > Tiles::GetElevationsRange() const {
	pair< Tile::elevation_t, Tile::elevation_t > result = { 0, 0 };
	Tile* tile;
	// determine min and max elevations from generated tiles
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
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
	Log( "Elevations range: min=" + to_string( result.first ) + " max=" + to_string( result.second ) );
	return result;
}

void Tiles::FixExtremeSlopes() {
	auto elevations_range = GetElevationsRange();
	util::Clamper<Tile::elevation_t> converter( Tile::ELEVATION_MIN, Tile::ELEVATION_MAX, elevations_range.first, elevations_range.second );
	RemoveExtremeSlopes( abs( converter.Clamp( Map::s_consts.tile.maximum_allowed_slope_elevation ) ) );
}

void Tiles::NormalizeElevationRange() {
	auto elevations_range = GetElevationsRange();
	util::Clamper<Tile::elevation_t> converter( elevations_range.first, elevations_range.second, Tile::ELEVATION_MIN, Tile::ELEVATION_MAX );
	
	Tile* tile;

	// convert every vertex to valid range
	for ( auto y = 0 ; y < m_height ; y++ ) {
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = At( x, y );
			tile->elevation_data.bottom = converter.Clamp( tile->elevation_data.bottom );
			tile->elevation_data.center = converter.Clamp( tile->elevation_data.center );
		}
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
	Tile::elevation_t elevation_fixby_change = 20;
	Tile::elevation_t elevation_fixby_max = max_allowed_diff / 3; // to prevent infinite loops when it grows so large it starts creating extreme slopes
	float elevation_fixby_div_change = 0.1f; // needed to prevent infinite loops when nearby tiles keep 'fixing' each other forever
	size_t pass = 0;
	Tile* tile;
	Tile::elevation_t elevation_fixby = 0;
	float elevation_fixby_div = 1.0f;
	bool found = true;
	size_t sz = m_width * m_height;
	size_t x, y, i;
	while ( found ) {
		if ( elevation_fixby < elevation_fixby_max ) {
			elevation_fixby += elevation_fixby_change;
		}
		elevation_fixby_div += elevation_fixby_div_change;
		Log( "Checking/fixing extreme slopes (pass " + to_string( ++pass ) + ")" );
		found = false;
		
		// don't run in normal cycle because it can give terrain some straight edges, go in random order instead
		// assume that on average we'll hit all tiles (but skipping some is no big deal)
		for ( i = 0 ; i < sz ; i++ ) {
			x = rand() % m_width;
			y = rand() % m_height;
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = At( x, y );

			#define x( _a, _b ) \
				while ( abs( *tile->elevation._a - *tile->elevation._b ) > max_allowed_diff ) { \
					/*Log( "fixing slope: " + to_string( *tile->elevation._a ) + "," + to_string( *tile->elevation._b ) + " / " + to_string( elevation_fixby ) );*/ \
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
		top_bottom_elevation = rand() % 2 ? 1 : -1;
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
		for ( auto x = 0 ; x < m_width ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = At( x, y );
			
			tile->Update();
		}
	}
}


}
}
}
