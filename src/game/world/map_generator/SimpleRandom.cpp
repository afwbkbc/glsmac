#include "SimpleRandom.h"

namespace game {
namespace world {
namespace map_generator {

void SimpleRandom::Generate( Tiles* tiles ) {
	Log( "Generating terrain ( " + to_string( tiles->GetWidth() ) + " x " + to_string( tiles->GetHeight() ) + " )" );
	
	Tile* tile;
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = 0 ; x < tiles->GetWidth() ; x++ ) {
			tile = tiles->At( x, y );
		
			ASSERT( tile->elevation.right, "tile right vertex not linked" );
			ASSERT( tile->elevation.left, "tile left vertex not linked" );
			ASSERT( tile->elevation.top, "tile top vertex not linked" );
			
#define RND Tile::ELEVATION_MIN + rand() % ( Tile::ELEVATION_MAX - Tile::ELEVATION_MIN + 1 )
			
			*tile->elevation.top = RND;
			*tile->elevation.left = RND;
			*tile->elevation.right = RND;
			tile->elevation.bottom = RND;
			
			tile->moisture = rand() % 3;
			tile->rockyness = rand() % 3;
			tile->features = rand() % 65536;
			
		}
	}
	
	// update averages
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = 0 ; x < tiles->GetWidth() ; x++ ) {
			tile = tiles->At( x, y );
			
			tile->elevation.center = ( *tile->elevation.left + *tile->elevation.top + *tile->elevation.right + tile->elevation.bottom ) / 4;
		}
	}
	
}

}
}
}
