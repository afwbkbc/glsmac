#include "Test.h"

namespace game {
namespace world {
namespace map_generator {

void Test::Generate( Tiles* tiles ) {
	Log( "Generating terrain ( " + to_string( tiles->GetWidth() ) + " x " + to_string( tiles->GetHeight() ) + " )" );

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
	
	SE( 1, 1, 500 );
	SE( 3, 1, 1500 );
	SE( 5, 1, 2500 );
	SE( 7, 1, 3500 );
	
	SE( 0, 4, -500 );
	SE( 2, 4, -1500 );
	SE( 4, 4, -2500 );
	SE( 6, 4, -3500 );
	
}

}
}
}
