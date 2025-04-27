#include "Tile.h"

#include "gse/value/Object.h"
#include "gse/value/Array.h"
#include "gse/value/Int.h"
#include "gse/value/Bool.h"
#include "gse/value/Null.h"
#include "gse/value/Undefined.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"

#include "game/backend/unit/Unit.h"
#include "game/backend/base/Base.h"

namespace game {
namespace backend {
namespace map {
namespace tile {

const std::unordered_map< direction_t, std::string > Tile::s_direction_str = {
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

const types::Buffer Tile::Serialize() const {
	types::Buffer buf;

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

	buf.WriteInt( yields.size() );
	for ( const auto& y : yields ) {
		buf.WriteString( y.first );
		buf.WriteInt( y.second );
	}

	return buf;
}

void Tile::Unserialize( types::Buffer buf ) {

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

	ASSERT_NOLOG( yields.empty(), "yields not empty" );
	const auto yields_size = buf.ReadInt();
	yields.reserve( yields_size );
	for ( size_t i = 0 ; i < yields_size ; i++ ) {
		yields.insert({
			buf.ReadString(),
			buf.ReadInt()
		});
	}

	Update();
}

const std::string Tile::ToString() const {
	return "@[ " + std::to_string( coord.x ) + " " + std::to_string( coord.y ) + " ]";
}

#define GETN( _n ) \
{ \
	"get_" #_n, \
	NATIVE_CALL( this ) { return _n->Wrap( GSE_CALL ); } ) \
}

WRAPIMPL_BEGIN( Tile )
	WRAPIMPL_PROPS
		{
			"x",
			VALUE( gse::value::Int,, coord.x )
		},
		{
			"y",
			VALUE( gse::value::Int,, coord.y )
		},
		{
			"is_locked",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS(0);
				return VALUE( gse::value::Bool,, m_is_locked );
			} )
		},
		{
			"is_water",
			VALUE( gse::value::Bool,, is_water_tile )
		},
		{
			"is_land",
			VALUE( gse::value::Bool,, !is_water_tile )
		},
		{
			"moisture",
			VALUE( gse::value::Int,, moisture )
		},
		{
			"rockiness",
			VALUE( gse::value::Int,, rockiness )
		},
		{
			"elevation",
			VALUE( gse::value::Int,, *elevation.center )
		},
		{
			"is_rocky",
			VALUE( gse::value::Bool,, rockiness == ROCKINESS_ROCKY )
		},
		{
			"has_fungus",
			VALUE( gse::value::Bool,, features & FEATURE_XENOFUNGUS )
		},
		{
			"has_river",
			VALUE( gse::value::Bool,, features & FEATURE_RIVER )
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
				N_EXPECT_ARGS( 1 );
				N_GETVALUE_UNWRAP( other, 0, Tile );
				return VALUE( gse::value::Bool,, IsAdjactentTo( other ) );
			})
		},
		{
			"get_surrounding_tiles",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				gse::value::array_elements_t result = {};
				for ( const auto& n : neighbours ) {
					result.push_back( n->Wrap( GSE_CALL ) );
				}
				return VALUE( gse::value::Array,, result );
			})
		},
		{
			"get_units",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				gse::value::array_elements_t result = {};
				for ( auto& it : units ) {
					result.push_back( it.second->Wrap( GSE_CALL ) );
				}
				return VALUE( gse::value::Array,, result );
			} )
		},
		{
			"get_base",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				if ( base ) {
					return base->Wrap( GSE_CALL );
				}
				else {
					return VALUE( gse::value::Null );
				}
			} )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Tile )

void Tile::Lock( const size_t initiator_slot ) {
	ASSERT_NOLOG( !m_is_locked, "tile already locked" );
	m_lock_initiator_slot = initiator_slot;
	m_is_locked = true;
}
void Tile::Unlock() {
	ASSERT_NOLOG( m_is_locked, "tile not locked" );
	m_is_locked = false;
}
const bool Tile::IsLocked() const {
	return m_is_locked;
}
const bool Tile::IsLockedBy( const size_t initiator_slot ) const {
	return m_is_locked && m_lock_initiator_slot == initiator_slot;
}

}
}
}
}
