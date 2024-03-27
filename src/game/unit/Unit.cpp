#include "Unit.h"

#include "gse/type/Object.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/callable/Native.h"

#include "game/Game.h"
#include "game/State.h"

#include "StaticDef.h"

namespace game {
namespace unit {

const Unit::health_t Unit::HEALTH_MIN = 0.0f;
const Unit::health_t Unit::HEALTH_MAX = 1.0f;
const Unit::health_t Unit::HEALTH_PER_TURN = 0.1f;

static size_t next_id = 1;
const size_t Unit::GetNextId() {
	return next_id;
}
const void Unit::SetNextId( const size_t id ) {
	next_id = id;
}

Unit::Unit( const size_t id, Def* def, Slot* owner, map::Tile* tile, const movement_t movement, const Morale::morale_t morale, const health_t health )
	: m_id( id )
	, m_def( def )
	, m_owner( owner )
	, m_tile( tile )
	, m_movement( movement )
	, m_morale( morale )
	, m_health( health ) {
	if ( next_id <= id ) {
		next_id = id + 1;
	}
}

const Unit::movement_t Unit::MINIMUM_MOVEMENT_TO_KEEP = 0.1f;

const bool Unit::HasMovesLeft() const {
	return m_movement >= unit::Unit::MINIMUM_MOVEMENT_TO_KEEP;
}

void Unit::OnTurn() {
	// TODO: move to gse?

	ASSERT_NOLOG( m_def->m_type == Def::DT_STATIC, "only static units are supported for now" );
	const auto* def = (StaticDef*)m_def;

	// refresh moves
	if ( def->m_movement_type != StaticDef::MT_IMMOVABLE ) {
		m_movement = def->m_movement_per_turn;
	}

	// replenish some health
	if ( m_health < HEALTH_MAX ) {
		m_health = std::min( m_health + HEALTH_PER_TURN, HEALTH_MAX );
	}
}

const std::string& Unit::GetMoraleString() {
	return m_def->m_moraleset->m_morale_values.at( m_morale ).m_name;
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
	return buf;
}

Unit* Unit::Unserialize( types::Buffer& buf, const Game* game ) {
	const auto id = buf.ReadInt();
	auto defbuf = types::Buffer( buf.ReadString() );
	auto* def = Def::Unserialize( defbuf );
	auto* slot = game ? &game->GetState()->m_slots.GetSlot( buf.ReadInt() ) : nullptr;
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	auto* tile = game ? game->GetMap()->GetTile( pos_x, pos_y ) : nullptr;
	const auto movement = (movement_t)buf.ReadFloat();
	const auto morale = (Morale::morale_t)buf.ReadInt();
	const auto health = (health_t)buf.ReadFloat();
	return new Unit( id, def, slot, tile, movement, morale, health );
}

WRAPIMPL_DYNAMIC_GETTERS( Unit, CLASS_UNIT )
	WRAPIMPL_DYNAMIC_GET( "id", Int, m_id )
	WRAPIMPL_DYNAMIC_GET( "movement", Float, m_movement )
	WRAPIMPL_DYNAMIC_GET( "morale", Int, m_morale )
	WRAPIMPL_DYNAMIC_GET( "health", Float, m_health )
	WRAPIMPL_DYNAMIC_GET( "is_immovable", Bool, m_def->GetMovementType() == Def::MT_IMMOVABLE )
	WRAPIMPL_DYNAMIC_GET( "is_land", Bool, m_def->GetMovementType() == Def::MT_LAND )
	WRAPIMPL_DYNAMIC_GET( "is_water", Bool, m_def->GetMovementType() == Def::MT_WATER )
	WRAPIMPL_DYNAMIC_GET( "is_air", Bool, m_def->GetMovementType() == Def::MT_AIR )
	WRAPIMPL_DYNAMIC_LINK( "get_def", m_def )
	WRAPIMPL_DYNAMIC_LINK( "get_owner", m_owner )
	WRAPIMPL_DYNAMIC_LINK( "get_tile", m_tile )
WRAPIMPL_DYNAMIC_SETTERS( Unit )
	WRAPIMPL_DYNAMIC_SET( "movement", Float, m_movement )
WRAPIMPL_DYNAMIC_END()

UNWRAPIMPL_PTR( Unit )

}
}
