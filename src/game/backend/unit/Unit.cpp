#include "Unit.h"

#include "gse/context/Context.h"
#include "gse/type/Object.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/Bool.h"
#include "gse/type/Undefined.h"
#include "gse/callable/Native.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/map/Map.h"
#include "MoraleSet.h"
#include "StaticDef.h"
#include "UnitManager.h"

namespace game {
namespace backend {
namespace unit {

static size_t next_id = 1;
const size_t Unit::GetNextId() {
	return next_id;
}
const void Unit::SetNextId( const size_t id ) {
	next_id = id;
}

Unit::Unit(
	UnitManager* um,
	const size_t id,
	Def* def,
	slot::Slot* owner,
	map::tile::Tile* tile,
	const movement_t movement,
	const morale_t morale,
	const health_t health,
	const bool moved_this_turn
)
	: MapObject( um->GetMap(), tile )
	, m_um( um )
	, m_id( id )
	, m_def( def )
	, m_owner( owner )
	, m_movement( movement )
	, m_morale( morale )
	, m_health( health )
	, m_moved_this_turn( moved_this_turn ) {
	if ( next_id <= id ) {
		next_id = id + 1;
	}
	SetTile( tile );
}

const movement_t Unit::MINIMUM_MOVEMENT_TO_KEEP = 0.1f;
const movement_t Unit::MINIMUM_HEALTH_TO_KEEP = 0.1f;

const bool Unit::HasMovesLeft() const {
	return m_movement >= unit::Unit::MINIMUM_MOVEMENT_TO_KEEP;
}

const std::string& Unit::GetMoraleString() const {
	return m_def->m_moraleset->m_morale_values.at( m_morale ).m_name;
}

void Unit::SetTile( map::tile::Tile* tile ) {
	if ( m_tile ) {
		m_tile->units.erase( m_id );
	}
	ASSERT_NOLOG( tile->units.find( m_id ) == tile->units.end(), "duplicate unit id in tile" );
	tile->units.insert(
		{
			m_id,
			this
		}
	);
	m_tile = tile;
	m_um->RefreshUnit( this );
}

const types::Buffer Unit::Serialize( const Unit* unit ) {
	types::Buffer buf;
	buf.WriteInt( unit->m_id );
	buf.WriteString( Def::Serialize( unit->m_def ).ToString() );
	buf.WriteInt( unit->m_owner->GetIndex() );
	buf.WriteInt( unit->m_tile->coord.x );
	buf.WriteInt( unit->m_tile->coord.y );
	buf.WriteFloat( unit->m_movement );
	buf.WriteInt( unit->m_morale );
	buf.WriteFloat( unit->m_health );
	buf.WriteBool( unit->m_moved_this_turn );
	return buf;
}

Unit* Unit::Unserialize( types::Buffer& buf, UnitManager* units ) {
	const auto id = buf.ReadInt();
	auto defbuf = types::Buffer( buf.ReadString() );
	auto* def = Def::Unserialize( defbuf );
	auto* slot = units ? units->GetSlot( buf.ReadInt() ) : nullptr;
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	auto* tile = units ? units->GetMap()->GetTile( pos_x, pos_y ) : nullptr;
	const auto movement = (movement_t)buf.ReadFloat();
	const auto morale = (morale_t)buf.ReadInt();
	const auto health = (health_t)buf.ReadFloat();
	const auto moved_this_turn = buf.ReadBool();
	return new Unit( units, id, def, slot, tile, movement, morale, health, moved_this_turn );
}

WRAPIMPL_DYNAMIC_GETTERS( Unit )
	WRAPIMPL_GET( "id", Int, m_id )
	WRAPIMPL_GET( "movement", Float, m_movement )
	WRAPIMPL_GET( "morale", Int, m_morale )
	WRAPIMPL_GET( "health", Float, m_health )
	WRAPIMPL_GET( "moved_this_turn", Bool, m_moved_this_turn )
	WRAPIMPL_GET( "is_immovable", Bool, m_def->GetMovementType() == MT_IMMOVABLE )
	WRAPIMPL_GET( "is_land", Bool, m_def->GetMovementType() == MT_LAND )
	WRAPIMPL_GET( "is_water", Bool, m_def->GetMovementType() == MT_WATER )
	WRAPIMPL_GET( "is_air", Bool, m_def->GetMovementType() == MT_AIR )
	WRAPIMPL_LINK( "get_def", m_def )
	WRAPIMPL_LINK( "get_owner", m_owner )
	WRAPIMPL_LINK( "get_tile", m_tile )
	{
		"set_tile",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( tile, 0, map::tile::Tile );
			if ( tile != m_tile ) {
				SetTile( tile );
			}
			return VALUE( gse::type::Undefined );
		} )
	},
	{
		"move_to_tile",
		NATIVE_CALL(this) {
			N_EXPECT_ARGS( 2 );
			N_GETVALUE_UNWRAP( tile, 0, map::tile::Tile );
			N_PERSIST_CALLABLE( on_complete, 1 );
			const auto* errmsg = m_um->MoveUnitToTile( this, tile, [ on_complete, &ctx, &si, &ep ]() {
				on_complete->Run( GSE_CALL, {} );
				N_UNPERSIST_CALLABLE( on_complete );
			});
			if ( errmsg ) {
				throw gse::Exception( gse::EC.GAME_ERROR, *errmsg, GSE_CALL );
				delete errmsg;
			}
			return VALUE( gse::type::Undefined );
		} )
	},
WRAPIMPL_DYNAMIC_SETTERS( Unit )
	WRAPIMPL_SET( "movement", Float, m_movement )
	WRAPIMPL_SET( "health", Float, m_health )
	WRAPIMPL_SET( "moved_this_turn", Bool, m_moved_this_turn )
WRAPIMPL_DYNAMIC_ON_SET( Unit )
	// this is potentially risky because if it gets zero health it will be despawned without script's awareness, how to handle it?
	// maybe despawn unit from within script? but then it would be script's responsibility to ensure there are no zero-health units walking around
	m_um->RefreshUnit( this );
WRAPIMPL_DYNAMIC_END()

UNWRAPIMPL_PTR( Unit )

}
}
}
