#include "Tile.h"

#include "gse/type/Object.h"
#include "gse/type/Int.h"
#include "gse/type/Bool.h"
#include "gse/callable/Native.h"

#include "game/unit/Unit.h"

namespace game {
namespace map {

void Tile::Update() {

	*elevation.center = ( *elevation.left + *elevation.top + *elevation.right + *elevation.bottom ) / 4;

	uint8_t corners_in_water = *elevation.center < ELEVATION_LEVEL_COAST
		? 1
		: 0;
	for ( auto& c : elevation.corners ) {
		if ( *c < ELEVATION_LEVEL_COAST ) {
			corners_in_water++;
		}
	}

	is_water_tile = corners_in_water > 2;
}

void Tile::Clear() {
	for ( auto& c : elevation.corners ) {
		*c = 0;
	}
	moisture = rockiness = bonus = features = terraforming = is_water_tile = 0;
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
	buf.WriteInt( rockiness );
	buf.WriteInt( bonus );

	buf.WriteInt( features );
	buf.WriteInt( terraforming );

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
	rockiness = buf.ReadInt();
	bonus = buf.ReadInt();

	features = buf.ReadInt();
	terraforming = buf.ReadInt();

	Update();
}

const std::string Tile::ToString() const {
	return "@[ " + std::to_string( coord.x ) + " " + std::to_string( coord.y ) + " ]";
}

#define GETN( _n ) \
{ \
	"get_" #_n, \
	NATIVE_CALL( this ) { return _n->Wrap(); } ) \
}

WRAPIMPL_BEGIN( Tile, CLASS_TILE )
	WRAPIMPL_PROPS {
		{
			"x",
			VALUE( gse::type::Int, coord.x )
		},
		{
			"y",
			VALUE( gse::type::Int, coord.y )
		},
		{
			"is_water",
			VALUE( gse::type::Bool, is_water_tile )
		},
		{
			"is_land",
			VALUE( gse::type::Bool, !is_water_tile )
		},
		GETN( W ),
		GETN( NW ),
		GETN( N ),
		GETN( NE ),
		GETN( E ),
		GETN( SE ),
		GETN( S ),
		GETN( SW ),
		{
			"get_units",
			NATIVE_CALL( this ) {
				gse::type::Object::properties_t result = {};
				for ( auto& it : units ) {
					result.insert_or_assign( std::to_string( it.second->m_id ), it.second->Wrap() );
				}
				return VALUE( gse::type::Object, result );
			} )
		}
	};
WRAPIMPL_END_PTR( Tile )

UNWRAPIMPL_PTR( Tile )

}
}
