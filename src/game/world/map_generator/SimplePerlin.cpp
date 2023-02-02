#include "SimplePerlin.h"

#include <ctime>
#include <cmath>
#include <chrono>

#include "util/Perlin.h"
#include "util/Clamper.h"

// higher values generate more interesting maps, at cost of longer map generation (isn't noticeable before 200 or so)
#define PERLIN_PASSES 128

using namespace chrono;

namespace game {
namespace world {
namespace map_generator {

void SimplePerlin::Generate( Tiles* tiles ) {
	Log( "Generating terrain ( " + to_string( tiles->GetWidth() ) + " x " + to_string( tiles->GetHeight() ) + " )" );
	
	util::Clamper<float> perlin_to_elevation( -1.0, 1.0, MAPGEN_ELEVATION_MIN, MAPGEN_ELEVATION_MAX );
	
	util::Clamper<float> perlin_to_value; // to moisture or rockyness
	perlin_to_value.SetRange( -1.0, 1.0, 1, 3 );
	
	auto now = chrono::high_resolution_clock::now();
	auto seed = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
	
	Log( "Map seed is " + to_string( seed ) );
	
	util::Perlin perlin( seed );
	
	Tile* tile;
	
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		
		if ( y == 0 ) {
			continue; // top row should have zero elevation // TODO: fix universally in some place
		}
		
		for ( auto x = 0 ; x < tiles->GetWidth() ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = tiles->At( x, y );
			
#define PERLIN_S( _x, _y, _z, _scale ) perlin.Noise( (float) ( (float)_x ) * _scale, (float) ( (float)_y ) * _scale, _z * _scale, PERLIN_PASSES )
#define PERLIN( _x, _y, _z ) PERLIN_S( _x, _y, _z, 1.0f )
			
			*tile->elevation.left = *tile->elevation.top = *tile->elevation.right = *tile->elevation.bottom = *tile->elevation.center = 0;
			
			const float z_elevation = 50;
			
			*tile->elevation.left = perlin_to_elevation.Clamp( PERLIN( x, y + 0.5f, z_elevation ) );
			*tile->elevation.top = perlin_to_elevation.Clamp( PERLIN( x + 0.5f, y, z_elevation ) );
			*tile->elevation.right = perlin_to_elevation.Clamp( PERLIN( x + 1.0f, y + 0.5f, z_elevation ) );
			*tile->elevation.bottom = perlin_to_elevation.Clamp( PERLIN( x + 0.5f, y + 1.0f, z_elevation ) );
			
			tile->Update();
		}
	}

	// start new cycle because we want all tiles have updated averages and dynamic properties
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = 0 ; x < tiles->GetWidth() ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = tiles->At( x, y );

			const float z_rocks = 70;
			const float z_moisture = 20;
			const float z_jungle = 130;
			const float z_xenofungus = 240;

			tile->rockyness = perlin_to_value.Clamp( round( PERLIN_S( x, y, z_rocks, 1.0f ) ) );
			if ( tile->rockyness == Tile::R_ROCKY ) {
				if ( rand() % 3 == 0 ) {
					tile->rockyness = Tile::R_ROLLING;
				}
			}
			
			tile->moisture = perlin_to_value.Clamp( ceil( PERLIN_S( x, y, z_moisture, 0.6f ) ) );
			if ( tile->moisture == Tile::M_RAINY ) {
				if ( PERLIN_S( x, y, z_jungle, 0.2f ) > 0.7 ) {
					tile->features |= Tile::F_JUNGLE;
				}
			}
			
			if ( PERLIN_S( x, y, z_xenofungus, 0.6f ) > 0.3 ) {
				tile->features |= Tile::F_XENOFUNGUS;
			}

			if ( rand() % 30 == 0 ) {
				tile->rockyness = Tile::R_ROCKY;
				for ( auto& t : tile->neighbours ) {
					if ( rand() % 3 != 0 ) {
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
