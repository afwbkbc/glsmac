#include "Tile.h"

namespace game {
namespace world {
namespace map {

Tile* Tile::Around( tile_cb_t cb, bool need_all ) {
	cb( W );
	if ( need_all || NW != this )
		cb( NW );
	if ( need_all || N != this )
		cb( N );
	if ( need_all || NE != this )
		cb( NE );
	cb( E );
	if ( need_all || SE != this )
		cb( SE );
	if ( need_all || S != this )
		cb( S );
	if ( need_all || SW != this )
		cb( SW );
	return this;
}

Tile* Tile::SelfAndAround( tile_cb_t cb ) {
	cb( this );
	return Around( cb );
}

}
}
}
