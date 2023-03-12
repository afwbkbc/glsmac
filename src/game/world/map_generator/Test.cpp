#include "Test.h"

namespace game {
namespace world {
namespace map_generator {

void Test::Generate( Tiles* tiles, size_t seed ) {
	Log( "Generating terrain ( " + std::to_string( tiles->GetWidth() ) + " x " + std::to_string( tiles->GetHeight() ) + " )" );

	{
		for ( size_t y = 0 ; y < tiles->GetHeight() ; y++ ) {
			for ( size_t x = y & 1 ; x < tiles->GetWidth() ; x += 2 ) {
				auto tile = tiles->At( x, y );
				
				for ( auto& c : tile->elevation.corners ) {
					*c = 100;
				}
				
				tile->moisture = Tile::M_ARID;
				tile->rockyness = Tile::R_FLAT;
				
				if ( tiles->GetRandom()->IsLucky( 2 ) ) {
					tile->features |= Tile::F_RIVER;
				}
				
			}
		}
	}
	
}

}
}
}
