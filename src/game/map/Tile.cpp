#include "Tile.h"

#include "gse/type/Object.h"
#include "gse/type/Array.h"
#include "gse/type/Int.h"
#include "gse/type/Bool.h"
#include "gse/callable/Native.h"

#include "game/unit/Unit.h"

namespace game {
namespace map {

const std::unordered_map< Tile::direction_t, std::string > Tile::s_direction_str = {
#define X( _x ) { D_##_x, #_x },
	{
		X( W )
		X( NW )
		X( N )
		X( NE )
		X( E )
		X( SE )
		X( S )
		X( SW )
	}
#undef X
};

const std::string& Tile::GetDirectionString( const direction_t direction ) {
	const auto& s = s_direction_str.find( direction );
	ASSERT_NOLOG( s != s_direction_str.end(), "unknown direction: " + std::to_string( direction ) );
	return s->second;
}

Tile* Tile::GetNeighbour( const direction_t direction ) {
	switch ( direction ) {
		case D_NONE: return this;
		case D_W: return W;
		case D_NW: return NW;
		case D_N: return N;
		case D_NE: return NE;
		case D_E: return E;
		case D_SE: return SE;
		case D_S: return S;
		case D_SW: return SW;
		default:
			THROW( "unknown tile direction: " + std::to_string( direction ) );
	}
}

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

const bool Tile::IsAdjactentTo( const Tile* other ) const {
	for ( const auto& n : neighbours ) {
		if ( n == other ) {
			return true;
		}
	}
	return false;
}

const float Tile::GetMovementCost() const {
	// TODO: move to scripts
	const bool is_native = true; // TODO
	if ( features & F_XENOFUNGUS ) {
		if ( is_native ) {
			return is_water_tile ? 1.0f : 1.0f / 3;
		}
		else {
			return 3.0f;
		}
	}
	if ( is_water_tile ) {
		return 1.0f;
	}
	// TODO: roads
	return 1.0f;
}

const float Tile::GetMovementAftercost() const {
	// TODO: move to scripts
	const bool is_native = true; // TODO
	if ( is_native && ( features & F_XENOFUNGUS ) ) {
		return 0.0f;
	}
	if ( !is_water_tile && rockiness == R_ROCKY ) {
		return 1.0f;
	}
	return 0.0f;
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
		{
			"is_rocky",
			VALUE( gse::type::Bool, rockiness == R_ROCKY )
		},
		{
			"has_fungus",
			VALUE( gse::type::Bool, features & F_XENOFUNGUS )
		},
		{
			"has_river",
			VALUE( gse::type::Bool, features & F_RIVER )
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
			"is_adjactent_to",
			NATIVE_CALL( this ) {
				N_ARGS( 1 );
				N_UNWRAP( other, 0, Tile );
				return VALUE( gse::type::Bool, IsAdjactentTo( other ) );
			})
		},
		{
			"get_surrounding_tiles",
			NATIVE_CALL( this ) {
				N_ARGS( 0 );
				gse::type::Array::elements_t result = {};
				for ( const auto& n : neighbours ) {
					result.push_back( n->Wrap() );
				}
				return VALUE( gse::type::Array, result );
			})
		},
		{
			"get_units",
			NATIVE_CALL( this ) {
				N_ARGS( 0 );
				gse::type::Array::elements_t result = {};
				for ( auto& it : units ) {
					result.push_back( it.second->Wrap() );
				}
				return VALUE( gse::type::Array, result );
			} )
		}
	};
WRAPIMPL_END_PTR( Tile )

UNWRAPIMPL_PTR( Tile )

}
}
