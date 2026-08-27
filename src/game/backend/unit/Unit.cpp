#include "Unit.h"

#include <cmath>

#include "gse/context/Context.h"
#include "gse/value/Object.h"
#include "gse/value/Int.h"
#include "gse/value/Float.h"
#include "gse/value/Bool.h"
#include "gse/value/Array.h"
#include "gse/value/Null.h"
#include "gse/value/Ptr.h"
#include "gse/callable/Native.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/map/Map.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/base/Base.h"
#include "game/backend/base/BaseManager.h"
#include "MoraleSet.h"
#include "StaticDef.h"
#include "UnitManager.h"
#include "gse/ExecutionPointer.h"
#include "util/String.h"

namespace game {
namespace backend {
namespace unit {

static const bool IsValidTerraformingOrder(
	const Def* const def,
	const map::tile::Tile* const tile,
	const map::tile::terraforming_t terraforming,
	const uint16_t turns_remaining
) {
	const bool is_supported_order =
		terraforming != map::tile::TERRAFORMING_NONE &&
		( terraforming & static_cast< map::tile::terraforming_t >( ~map::tile::TERRAFORMING_ALL ) ) == 0 &&
		( terraforming & ( terraforming - 1 ) ) == 0;
	return
		(
			terraforming == map::tile::TERRAFORMING_NONE &&
			turns_remaining == 0
		) ||
		(
			is_supported_order &&
			turns_remaining > 0 &&
			turns_remaining <= Unit::MAX_TERRAFORMING_TURNS &&
			def->m_can_terraform &&
			tile &&
			!tile->is_water_tile
		);
}

static size_t next_id = 1;
const size_t Unit::GetNextId() {
	return next_id;
}
const void Unit::SetNextId( const size_t id ) {
	next_id = id;
}

Unit::Unit(
	GSE_CALLABLE,
	UnitManager* um,
	const size_t id,
	Def* def,
	slot::Slot* owner,
	map::tile::Tile* tile,
	const movement_t movement,
	const morale_t morale,
	const health_t health,
	const bool moved_this_turn,
	const map::tile::terraforming_t terraforming,
	const uint16_t terraforming_turns_remaining,
	const size_t home_base_id,
	const uint16_t fuel,
	const size_t transport_id,
	const bool native_capture_attempted
)
	: MapObject( um->GetMap(), tile )
	, m_um( um )
	, m_id( id )
	, m_def( def )
	, m_owner( owner )
	, m_movement( movement )
	, m_morale( morale )
	, m_health( health )
	, m_moved_this_turn( moved_this_turn )
	, m_terraforming( terraforming )
	, m_terraforming_turns_remaining( terraforming_turns_remaining )
	, m_home_base_id( home_base_id )
	, m_fuel( fuel )
	, m_transport_id( transport_id )
	, m_native_capture_attempted( native_capture_attempted ) {
	if ( !IsValidTerraformingOrder( def, tile, terraforming, terraforming_turns_remaining ) ) {
		THROW( "invalid unit terraforming order" );
	}
	if (
		def->m_type != DT_STATIC ||
		fuel > static_cast< const StaticDef* >( def )->m_operational_range
	) {
		THROW( "invalid unit fuel" );
	}
	if ( transport_id == id ) {
		THROW( "unit cannot transport itself" );
	}
	if ( next_id <= id ) {
		next_id = id + 1;
	}
	SetTile( GSE_CALL, tile );
}

Unit::~Unit() {
	if ( !m_is_registered && m_tile ) {
		const auto it = m_tile->units.find( m_id );
		if ( it != m_tile->units.end() && it->second == this ) {
			m_tile->units.erase( it );
		}
	}
}

const movement_t Unit::MINIMUM_MOVEMENT_TO_KEEP = 0.025f;
const movement_t Unit::MINIMUM_HEALTH_TO_KEEP = 0.025f;

const bool Unit::HasMovesLeft() const {
	return m_movement >= unit::Unit::MINIMUM_MOVEMENT_TO_KEEP;
}

const std::string& Unit::GetMoraleString() const {
	return m_def->m_moraleset->m_morale_values.at( m_morale ).m_name;
}

void Unit::SetTile( GSE_CALLABLE, map::tile::Tile* tile ) {
	if ( m_tile ) {
		m_tile->units.erase( m_id );
	}
	ASSERT( tile->units.find( m_id ) == tile->units.end(), "duplicate unit id in tile" );
	tile->units.insert(
		{
			m_id,
			this
		}
	);
	m_tile = tile;
}

void Unit::SetTerraformingOrder(
	GSE_CALLABLE,
	const map::tile::terraforming_t terraforming,
	const uint16_t turns_remaining
) {
	m_um->m_game->CheckRW( GSE_CALL );
	if ( !IsValidTerraformingOrder( m_def, m_tile, terraforming, turns_remaining ) ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Invalid unit terraforming order" );
	}
	if ( m_terraforming != terraforming || m_terraforming_turns_remaining != turns_remaining ) {
		m_terraforming = terraforming;
		m_terraforming_turns_remaining = turns_remaining;
		m_um->RefreshUnit( GSE_CALL, this );
	}
}

void Unit::SetFuel( GSE_CALLABLE, const uint16_t fuel ) {
	m_um->m_game->CheckRW( GSE_CALL );
	ASSERT( m_def->m_type == DT_STATIC, "only static unit definitions support fuel" );
	const auto* const def = static_cast< const StaticDef* >( m_def );
	if ( fuel > def->m_operational_range ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Unit fuel exceeds its operational range" );
	}
	if ( m_fuel != fuel ) {
		m_fuel = fuel;
		m_um->RefreshUnit( GSE_CALL, this );
	}
}

void Unit::SetTransportId( const size_t transport_id ) {
	m_transport_id = transport_id;
	std::lock_guard guard( m_wrapobjs_mutex );
	for ( auto* const wrapobj : m_wrapobjs ) {
		const auto transport_it = wrapobj->value.find( "transport_id" );
		ASSERT( transport_it != wrapobj->value.end(), "unit wrapper has no transport_id property" );
		ASSERT( transport_it->second->type == gse::VT_INT, "unit transport_id property is not an int" );
		( (gse::value::Int*)transport_it->second )->value = m_transport_id;

		const auto embarked_it = wrapobj->value.find( "is_embarked" );
		ASSERT( embarked_it != wrapobj->value.end(), "unit wrapper has no is_embarked property" );
		ASSERT( embarked_it->second->type == gse::VT_BOOL, "unit is_embarked property is not a bool" );
		( (gse::value::Bool*)embarked_it->second )->value = m_transport_id != 0;
	}
}

const types::Buffer Unit::Serialize( const Unit* unit ) {
	types::Buffer buf;
	buf.WriteInt( unit->m_id );
	buf.WriteString( unit->m_def->m_id );
	buf.WriteInt( unit->m_owner->GetIndex() );
	buf.WriteInt( unit->m_tile->coord.x );
	buf.WriteInt( unit->m_tile->coord.y );
	buf.WriteFloat( unit->m_movement );
	buf.WriteInt( unit->m_morale );
	buf.WriteFloat( unit->m_health );
	buf.WriteBool( unit->m_moved_this_turn );
	buf.WriteInt( unit->m_terraforming );
	buf.WriteInt( unit->m_terraforming_turns_remaining );
	buf.WriteInt( unit->m_home_base_id );
	buf.WriteInt( unit->m_fuel );
	buf.WriteInt( unit->m_transport_id );
	buf.WriteBool( unit->m_native_capture_attempted );
	return buf;
}

Unit* Unit::Deserialize( GSE_CALLABLE, types::Buffer& buf, UnitManager* um ) {
	if ( !um ) {
		THROW( "cannot deserialize unit without a manager" );
	}
	const auto id = buf.ReadInt< size_t >( "unit id" );
	if ( id == 0 ) {
		THROW( "serialized unit id is zero" );
	}
	const auto def_id = buf.ReadString();
	auto* def = um->GetUnitDef( def_id );
	if ( !def ) {
		THROW( "could not find unit def: " + def_id );
	}
	const auto slot_num = buf.ReadInt< size_t >( "unit owner slot" );
	if ( slot_num >= um->m_game->GetState()->m_slots->GetCount() ) {
		THROW( "serialized unit owner slot is out of bounds" );
	}
	auto* slot = um->GetSlot( slot_num );
	if ( slot->GetState() != slot::Slot::SS_PLAYER ) {
		THROW( "serialized unit owner slot has no player" );
	}
	const auto pos_x = buf.ReadInt< size_t >( "unit tile x" );
	const auto pos_y = buf.ReadInt< size_t >( "unit tile y" );
	if (
		pos_x >= um->GetMap()->GetWidth() ||
		pos_y >= um->GetMap()->GetHeight() ||
		pos_x % 2 != pos_y % 2
	) {
		THROW( "invalid serialized unit tile" );
	}
	auto* tile = um->GetMap()->GetTile( pos_x, pos_y );
	if ( tile->units.find( id ) != tile->units.end() ) {
		THROW( "serialized unit id already exists on tile" );
	}
	const auto movement = buf.ReadFloat();
	const auto morale = buf.ReadInt< morale_t >( "unit morale" );
	const auto health = buf.ReadFloat();
	const auto moved_this_turn = buf.ReadBool();
	const auto terraforming = buf.ReadInt< map::tile::terraforming_t >( "unit terraforming order" );
	const auto terraforming_turns_remaining = buf.ReadInt< uint16_t >( "unit terraforming turns remaining" );
	const auto home_base_id = buf.GetRemaining() > 0
		? buf.ReadInt< size_t >( "unit home base id" )
		: 0;
	ASSERT( def->m_type == DT_STATIC, "only static unit definitions support fuel" );
	const auto* const staticdef = static_cast< const StaticDef* >( def );
	const auto fuel = buf.GetRemaining() > 0
		? buf.ReadInt< uint16_t >( "unit fuel" )
		: static_cast< uint16_t >( staticdef->m_operational_range );
	const auto transport_id = buf.GetRemaining() > 0
		? buf.ReadInt< size_t >( "unit transport id" )
		: 0;
	const auto native_capture_attempted = buf.GetRemaining() > 0
		? buf.ReadBool()
		: false;
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized unit" );
	}
	if ( !std::isfinite( movement ) || movement < 0.0f ) {
		THROW( "invalid serialized unit movement" );
	}
	if (
		morale < MORALE_MIN ||
		morale > MORALE_MAX ||
		static_cast< size_t >( morale ) >= def->m_moraleset->m_morale_values.size()
	) {
		THROW( "invalid serialized unit morale" );
	}
	if ( !std::isfinite( health ) || health <= 0.0f || health > StaticDef::HEALTH_MAX ) {
		THROW( "invalid serialized unit health" );
	}
	if ( !IsValidTerraformingOrder( def, tile, terraforming, terraforming_turns_remaining ) ) {
		THROW( "invalid serialized unit terraforming order" );
	}
	if ( fuel > staticdef->m_operational_range ) {
		THROW( "invalid serialized unit fuel" );
	}
	if ( transport_id == id ) {
		THROW( "serialized unit cannot transport itself" );
	}
	return new Unit(
		GSE_CALL,
		um,
		id,
		def,
		slot,
		tile,
		movement,
		morale,
		health,
		moved_this_turn,
		terraforming,
		terraforming_turns_remaining,
		home_base_id,
		fuel,
		transport_id,
		native_capture_attempted
	);
}

WRAPIMPL_SERIALIZE( Unit )
	buf->WriteInt( obj->m_id );
}

WRAPIMPL_DESERIALIZE( Unit )
	const auto id = buf->ReadInt< size_t >( "unit reference id" );
	const auto& unit = game->GetUM()->GetUnit( id );
	if ( !unit ) {
		THROW( "unit id not found: " + std::to_string( id ) );
	}
	return unit->Wrap( GSE_CALL );
}

WRAPIMPL_DYNAMIC_GETTERS( Unit )
	WRAPIMPL_GET_CUSTOM( "id", Int, m_id )
	WRAPIMPL_GET_CUSTOM( "def", String, m_def->m_id )
	WRAPIMPL_GET_CUSTOM( "owner", Int, m_owner->GetIndex() )
	WRAPIMPL_GET_CUSTOM( "tile", Object, GSE_CALL_NOGC, {
		{ "x", VALUE( gse::value::Int,, m_tile->coord.x ) },
		{ "y", VALUE( gse::value::Int,, m_tile->coord.y ) },
	} )
	WRAPIMPL_GET_PTR( "movement", m_movement )
	WRAPIMPL_GET_PTR( "morale", m_morale )
	WRAPIMPL_GET_PTR( "health", m_health )
	WRAPIMPL_GET_PTR( "moved_this_turn", m_moved_this_turn )
	WRAPIMPL_GET_CUSTOM( "terraforming", String, map::tile::Tile::GetTerraformingString( m_terraforming ) )
	WRAPIMPL_GET_CUSTOM( "terraforming_turns_remaining", Int, m_terraforming_turns_remaining )
	WRAPIMPL_GET_CUSTOM( "home_base_id", Int, m_home_base_id )
	WRAPIMPL_GET_CUSTOM( "fuel", Int, m_fuel )
	WRAPIMPL_GET_CUSTOM( "transport_id", Int, m_transport_id )
	WRAPIMPL_GET_PTR( "native_capture_attempted", m_native_capture_attempted )
	WRAPIMPL_GET_CUSTOM( "is_embarked", Bool, m_transport_id != 0 )
	WRAPIMPL_GET_CUSTOM( "is_immovable", Bool, m_def->GetMovementType() == MT_IMMOVABLE )
	WRAPIMPL_GET_CUSTOM( "is_land", Bool, m_def->GetMovementType() == MT_LAND )
	WRAPIMPL_GET_CUSTOM( "is_water", Bool, m_def->GetMovementType() == MT_WATER )
	WRAPIMPL_GET_CUSTOM( "is_air", Bool, m_def->GetMovementType() == MT_AIR )
	WRAPIMPL_LINK( "get_def", m_def )
	WRAPIMPL_LINK( "get_owner", m_owner )
	WRAPIMPL_LINK( "get_tile", m_tile )
	{
		"get_transport",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			if ( m_transport_id == 0 ) {
				return VALUE( gse::value::Null );
			}
			auto* const transport = m_um->GetUnit( m_transport_id );
			if ( !transport ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "Unit transport no longer exists" );
			}
			return transport->Wrap( GSE_CALL );
		} )
	},
	{
		"get_cargo",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			gse::value::array_elements_t result = {};
			for ( auto* const cargo : m_um->GetCargo( this ) ) {
				result.push_back( cargo->Wrap( GSE_CALL ) );
			}
			return VALUE( gse::value::Array, , result );
		} )
	},
	{
		"embark",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( transport, 0, Unit );
			m_um->EmbarkUnit( GSE_CALL, this, transport );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"disembark",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			m_um->DisembarkUnit( GSE_CALL, this );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"set_fuel",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 1 );
			N_GETVALUE( fuel, 0, Int );
			if ( fuel < 0 || fuel > StaticDef::MAX_OPERATIONAL_RANGE ) {
				GSE_ERROR( gse::EC.INVALID_CALL, "Invalid unit fuel" );
			}
			SetFuel( GSE_CALL, static_cast< uint16_t >( fuel ) );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"set_home_base_id",
		NATIVE_CALL( this ) {
			m_um->m_game->CheckRW( GSE_CALL );
			N_EXPECT_ARGS( 1 );
			N_GETVALUE( home_base_id, 0, Int );
			if ( home_base_id < 0 ) {
				GSE_ERROR( gse::EC.INVALID_CALL, "Home base ID cannot be negative" );
			}
			if ( home_base_id > 0 ) {
				const auto* const base = m_um->m_game->GetBM()->GetBase( static_cast< size_t >( home_base_id ) );
				if ( !base || base->m_owner != m_owner ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Home base must exist and belong to the unit owner" );
				}
			}
			m_home_base_id = static_cast< size_t >( home_base_id );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"move_to_tile",
		NATIVE_CALL( this ) {

			m_um->m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 2 );
			N_GETVALUE_UNWRAP( tile, 0, map::tile::Tile );
			N_GET_CALLABLE( on_complete, 1 );
			m_um->Persist( on_complete );
			const auto* errmsg = m_um->MoveUnitToTile( GSE_CALL, this, tile, [ this, on_complete, gc_space, ctx, si, ep ]() {
				auto ep2 = ep;
				on_complete->Run( gc_space, ctx, si, ep2, {} );
				m_um->Unpersist( on_complete );
			});
			if ( errmsg ) {
				GSE_ERROR( gse::EC.GAME_ERROR, *errmsg );
				m_um->Unpersist( on_complete );
				delete errmsg;
			}
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"teleport_to_tile",
		NATIVE_CALL( this ) {
			m_um->m_game->CheckRW( GSE_CALL );
			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( tile, 0, map::tile::Tile );
			const auto* errmsg = m_um->TeleportUnitToTile( GSE_CALL, this, tile );
			if ( errmsg ) {
				GSE_ERROR( gse::EC.GAME_ERROR, *errmsg );
				delete errmsg;
			}
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"set_terraforming_order",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 2 );
			N_GETVALUE( terraforming_name, 0, String );
			N_GETVALUE( turns_remaining, 1, Int );
			const auto terraforming = map::tile::Tile::GetTerraformingFromString( terraforming_name );
			if (
				( terraforming == map::tile::TERRAFORMING_NONE && util::String::GetLowerCase( terraforming_name ) != "none" ) ||
				turns_remaining < 0 ||
				turns_remaining > MAX_TERRAFORMING_TURNS
			) {
				GSE_ERROR( gse::EC.INVALID_CALL, "Invalid unit terraforming order" );
			}
			SetTerraformingOrder( GSE_CALL, terraforming, static_cast< uint16_t >( turns_remaining ) );
			return VALUE( gse::value::Undefined );
		} )
	},
WRAPIMPL_DYNAMIC_SETTERS( Unit )
	WRAPIMPL_SET_PTR( "movement", Float, m_movement )
	if ( key == "morale" ) {
		if ( value->type != gse::value::Int::GetType() ) {
			GSE_ERROR( gse::EC.INVALID_ASSIGNMENT, "Invalid assignment value type, expected: int, got: " + value->GetTypeString() );
		}
		obj->m_morale = obj->m_um->GetMorale( GSE_CALL, ( (gse::value::Int*)value )->value );
		obj->OnWrapSet( GSE_CALL, key );
		return;
	}
	WRAPIMPL_SET_PTR( "health", Float, m_health )
	WRAPIMPL_SET_PTR( "moved_this_turn", Bool, m_moved_this_turn )
	WRAPIMPL_SET_PTR( "native_capture_attempted", Bool, m_native_capture_attempted )
WRAPIMPL_DYNAMIC_ON_SET( Unit )
	// this is potentially risky because if it gets zero health it will be despawned without script's awareness, how to handle it?
	// maybe despawn unit from within script? but then it would be script's responsibility to ensure there are no zero-health units walking around
	m_um->RefreshUnit( GSE_CALL, this );
WRAPIMPL_DYNAMIC_END()

UNWRAPIMPL_PTR( Unit )

}
}
}
