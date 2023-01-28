#include "SimpleRandom.h"

namespace game {
namespace world {
namespace map_generator {

void SimpleRandom::Generate( Tiles* tiles ) {
	Log( "Generating terrain ( " + to_string( tiles->GetWidth() ) + " x " + to_string( tiles->GetHeight() ) + " )" );
	
	Tile* tile;
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = 0 ; x < tiles->GetWidth() ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			
			tile = tiles->At( x, y );
		
#define RND Tile::ELEVATION_MIN + rand() % ( Tile::ELEVATION_MAX - Tile::ELEVATION_MIN + 1 )
			
			*tile->elevation.top = RND;
			*tile->elevation.left = RND;
			*tile->elevation.right = RND;
			tile->elevation.bottom = RND;
			
			switch ( rand() % 6 ) {
				case 0:
				case 1:
				case 2:
					tile->moisture = Tile::M_MOIST;
					break;
				case 3:
				case 4:
					tile->moisture = Tile::M_RAINY;
					break;
				default:
					tile->moisture = Tile::M_MOIST;
			}
			
			tile->rockyness = rand() % 3;
			tile->features = rand() % 65536;
			
		}
	}
	
	// update averages (TODO: do automatically somewhere?)
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = 0 ; x < tiles->GetWidth() ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			
			tile = tiles->At( x, y );
			
			tile->elevation.center = ( *tile->elevation.left + *tile->elevation.top + *tile->elevation.right + tile->elevation.bottom ) / 4;
		}
	}
	
}

}
}
}
