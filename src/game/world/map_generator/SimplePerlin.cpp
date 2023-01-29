#include "SimplePerlin.h"

#include <ctime>
#include <cmath>
#include <chrono>

#include "util/Perlin.h"
#include "util/Clamper.h"

using namespace chrono;

namespace game {
namespace world {
namespace map_generator {

void SimplePerlin::Generate( Tiles* tiles ) {
	Log( "Generating terrain ( " + to_string( tiles->GetWidth() ) + " x " + to_string( tiles->GetHeight() ) + " )" );
	
	util::Clamper<float> perlin_to_elevation;
	perlin_to_elevation.SetRange( -1.0, 1.0, 0, 3500 );
	
	util::Clamper<float> perlin_to_value; // to moisture or rockyness
	perlin_to_value.SetRange( -1.0, 1.0, 1, 3 );
	
	auto now = chrono::high_resolution_clock::now();
	auto seed = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
	
	util::Perlin perlin( seed );
	
	Tile* tile;
	
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = 0 ; x < tiles->GetWidth() ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = tiles->At( x, y );
			
#define PERLIN_S( _x, _y, _z, _scale ) perlin.Noise( (float) ( (float)_x ) * _scale, (float) ( (float)_y ) * _scale, _z * _scale, 16 )
#define PERLIN( _x, _y, _z ) PERLIN_S( _x, _y, _z, 1.0f )
			
			*tile->elevation.left = *tile->elevation.top = *tile->elevation.right = tile->elevation.bottom = tile->elevation.center = 0;
			
			const float z_elevation = 50;
			const float z_rocks = 70;
			const float z_moisture = 20;
			const float z_jungle = 130;
			const float z_xenofungus = 150;
			
			*tile->elevation.left = perlin_to_elevation.Clamp( PERLIN( x, y, z_elevation ) );
			*tile->elevation.top = perlin_to_elevation.Clamp( PERLIN( x, y, z_elevation ) );
			*tile->elevation.right = perlin_to_elevation.Clamp( PERLIN( x + 1.0f, y, z_elevation ) );
			tile->elevation.bottom = perlin_to_elevation.Clamp( PERLIN( x, y + 1.0f, z_elevation ) );
			
			tile->Update();
			
			tile->rockyness = perlin_to_value.Clamp( round( PERLIN_S( x, y, z_rocks, 4.0f ) ) );
			
			tile->moisture = perlin_to_value.Clamp( ceil( PERLIN_S( x, y, z_moisture, 0.6f ) ) );
			if ( tile->moisture == Tile::M_RAINY ) {
				if ( PERLIN_S( x, y, z_jungle, 0.2f ) > 0.7 ) {
					tile->features |= Tile::F_JUNGLE;
				}
			}
			
			if ( PERLIN_S( x, y, z_xenofungus, 1.0f ) > 0.8 ) {
				tile->features |= Tile::F_XENOFUNGUS;
			}

			if ( rand() % 30 == 0 ) {
				tile->rockyness = Tile::R_ROCKY;
				tile->Around( TH() {
					if ( rand() % 10 == 0 ) {
						tile->rockyness = max( Tile::R_ROLLING, tile->rockyness );
					}
				});
			}
			
		}
	}
}

}
}
}
