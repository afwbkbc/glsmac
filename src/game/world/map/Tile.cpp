#include "Tile.h"

namespace game {
namespace world {
namespace map {

void Tile::Update() {
	
	*elevation.center = ( *elevation.left + *elevation.top + *elevation.right + *elevation.bottom ) / 4;
	
	uint8_t corners_in_water = *elevation.center < ELEVATION_LEVEL_COAST ? 1 : 0;
	for ( auto& c : elevation.corners ) {
		if ( *c < ELEVATION_LEVEL_COAST ) {
			corners_in_water++;
		}
	}
	
	is_water_tile = corners_in_water > 2;
}

const Buffer Tile::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( coord.x );
	buf.WriteInt( coord.y );

	buf.WriteInt( *elevation.center );
	buf.WriteInt( *elevation.left );
	buf.WriteInt( *elevation.top );
	buf.WriteInt( *elevation.right );
	buf.WriteInt( *elevation.bottom );
	
	buf.WriteInt( moisture );
	buf.WriteInt( rockyness );
	
	buf.WriteInt( features );
	
	return buf;
}

void Tile::Unserialize( Buffer buf ) {
	
	coord.x = buf.ReadInt();
	coord.y = buf.ReadInt();
	
	*elevation.center = buf.ReadInt();
	*elevation.left = buf.ReadInt();
	*elevation.top = buf.ReadInt();
	*elevation.right = buf.ReadInt();
	*elevation.center = buf.ReadInt();
	
	moisture = buf.ReadInt();
	rockyness = buf.ReadInt();
	
	features = buf.ReadInt();
	
}

}
}
}
