#include <ctime>
#include <cmath>
#include <algorithm>
#include <random>

#include "SimplePerlin.h"

#include "util/Perlin.h"
#include "util/Clamper.h"

// higher values generate more interesting maps, at cost of longer map generation (isn't noticeable before 200 or so)
#define PERLIN_PASSES 128

namespace game {
namespace world {
namespace map_generator {

void SimplePerlin::Generate( Tiles* tiles, size_t seed ) {
	Log( "Generating terrain ( " + std::to_string( tiles->GetWidth() ) + " x " + std::to_string( tiles->GetHeight() ) + " )" );
	
	const float land_bias = 1.3f; // increase amount of land generated
	util::Clamper<float> perlin_to_elevation( -1.0 - land_bias, 1.0, MAPGEN_ELEVATION_MIN, MAPGEN_ELEVATION_MAX );
	
	util::Clamper<float> perlin_to_value; // to moisture or rockyness
	perlin_to_value.SetRange( -1.0, 1.0, 1, 3 );
	
	util::Perlin perlin( seed );
	
	Tile* tile;
	
	// process in random order
	std::vector< Tile* > tiles_vec;
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = y & 1 ; x < tiles->GetWidth() ; x += 2 ) {
			tiles_vec.push_back( tiles->At( x, y ) );
		}
	}

	std::mt19937 g( seed );
	std::shuffle( tiles_vec.begin(), tiles_vec.end(), g );
	
	for ( auto& tile : tiles_vec ) {
		
#define PERLIN_S( _x, _y, _z, _scale ) perlin.Noise( (float) ( (float)_x ) * _scale, (float) ( (float)_y ) * _scale, _z * _scale, PERLIN_PASSES )
#define PERLIN( _x, _y, _z ) PERLIN_S( _x, _y, _z, 1.0f )
		
		*tile->elevation.left = *tile->elevation.top = *tile->elevation.right = *tile->elevation.bottom = *tile->elevation.center = 0;
		
		const float z_elevation = 0;
		
		*tile->elevation.left = perlin_to_elevation.Clamp( PERLIN( tile->coord.x, tile->coord.y + 0.5f, z_elevation ) );
		*tile->elevation.top = perlin_to_elevation.Clamp( PERLIN( tile->coord.x + 0.5f, tile->coord.y, z_elevation ) );
		*tile->elevation.right = perlin_to_elevation.Clamp( PERLIN( tile->coord.x + 1.0f, tile->coord.y + 0.5f, z_elevation ) );
		*tile->elevation.bottom = perlin_to_elevation.Clamp( PERLIN( tile->coord.x + 0.5f, tile->coord.y + 1.0f, z_elevation ) );
		
		tile->Update();
	}

	// start new cycle because we want all tiles have updated averages and dynamic properties
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = y & 1 ; x < tiles->GetWidth() ; x += 2 ) {
			tile = tiles->At( x, y );

			const float z_rocks = 1/70;
			const float z_moisture = 1/20;
			const float z_jungle = 1/130;
			const float z_xenofungus = 1/240;

			tile->rockyness = perlin_to_value.Clamp( round( PERLIN_S( x + 0.5f, y + 0.5f, z_rocks, 1.0f ) ) );
			if ( tile->rockyness == Tile::R_ROCKY ) {
				if ( tiles->GetRandom()->IsLucky( 3 ) ) {
					tile->rockyness = Tile::R_ROLLING;
				}
			}
			
			tile->moisture = perlin_to_value.Clamp( ceil( PERLIN_S( x + 0.5f, y + 0.5f, z_moisture, 0.6f ) ) );
			if ( tile->moisture == Tile::M_RAINY ) {
				if ( PERLIN_S( x + 0.5f, y + 0.5f, z_jungle, 0.2f ) > 0.7 ) {
					tile->features |= Tile::F_JUNGLE;
				}
			}
			
			if ( PERLIN_S( x + 0.5f, y + 0.5f, z_xenofungus, 0.6f ) > 0.4 ) {
				tile->features |= Tile::F_XENOFUNGUS;
			}

			if ( tiles->GetRandom()->IsLucky( 30 ) ) {
				tile->rockyness = Tile::R_ROCKY;
				for ( auto& t : tile->neighbours ) {
					if ( tiles->GetRandom()->IsLucky( 3 ) ) {
						if ( t->rockyness != Tile::R_ROCKY ) {
							t->rockyness = Tile::R_ROLLING;
						}
					}
				}
			}
			
		}
	}
	
	for ( size_t i = 0 ; i < 8 ; i++ ) {
		SmoothTerrain( tiles, ( i < 2 ), true ); // smooth land 2 times, sea 8 times
	}
	
	Finalize( tiles );
	
}

}
}
}
