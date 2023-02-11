#include "Test.h"

namespace game {
namespace world {
namespace map_generator {

void Test::Generate( Tiles* tiles, size_t seed ) {
	Log( "Generating terrain ( " + std::to_string( tiles->GetWidth() ) + " x " + std::to_string( tiles->GetHeight() ) + " )" );

#define SE( _x, _y, _e ) { \
	tile = tiles->At( _x, _y ); \
	*tile->elevation.left = _e; \
	*tile->elevation.right = _e; \
	*tile->elevation.top = _e; \
	tile->elevation.bottom = _e; \
	tile->elevation.center = _e; \
}
	
	ASSERT( tiles->GetWidth() > 7, "width too small for testing" );
	ASSERT( tiles->GetHeight() > 7, "height too small for testing" );
	
	Tile* tile;
	
	{
		// everything arid by default
		for ( size_t y = 0 ; y < tiles->GetWidth() ; y++ ) {
			for ( size_t x = 0 ; x < tiles->GetWidth() ; x++ ) {
				if ( y % 2 != x % 2 ) {
					continue;
				}
				
				tiles->At( x, y )->moisture = Tile::M_ARID;
			}
		}
	}
	
#define SLOPE( _x, _y, _val ) \
	tile = tiles->At( _x, _y ); \
	*tile->elevation.left = ( _val + 400 ); \
	*tile->elevation.top = ( _val + 400 ); \
	*tile->elevation.right = ( _val - 400 ); \
	*tile->elevation.bottom = ( _val - 400 ); \
	*tile->E->elevation.right = *tile->elevation.right; \
	*tile->W->elevation.left = *tile->elevation.left; \
	tile->Update();
	
	SLOPE( 2, 2, 2000 );
	SLOPE( 4, 4, 1200 );
	SLOPE( 6, 6, 400 );
	SLOPE( 8, 8, -400 );
	SLOPE( 10, 10, -1200 );
	SLOPE( 12, 12, -2000 );
	
/*	
	{ // isolated tiles
		tile = tiles->At( 1, 1 );
		tile->moisture = Tile::M_ARID;
		tile->rockyness = Tile::R_ROCKY;

		tile = tiles->At( 3, 1 );
		tile->moisture = Tile::M_MOIST;
		tile->rockyness = Tile::R_ROLLING;

		tile = tiles->At( 5, 1 );
		tile->moisture = Tile::M_RAINY;
		tile->rockyness = Tile::R_FLAT;
	}
*/
/*
	{ // small rainy area
		/*tiles->At( 5, 5 )->SelfAndAround( TH() {
			tile->moisture = Tile::M_RAINY;
		});*
		
		Tile::moisture_t m = Tile::M_RAINY;
		
		tile = tiles->At( 4, 4 );
		//tile->moisture = m;
		tile->Around( TH( m ) {
			tile->moisture = m;
		});
		
		
	}
*/


}

}
}
}
