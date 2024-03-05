#include "Unit.h"

#include "gse/type/Object.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/callable/Native.h"

#include "game/Game.h"
#include "game/State.h"

namespace game {
namespace unit {

// must be hardcoded because unit badge sprites are
const Unit::morale_t Unit::MORALE_MIN = 1;
const Unit::morale_t Unit::MORALE_MAX = 7;

const Unit::health_t Unit::HEALTH_MIN = 0.0f;
const Unit::health_t Unit::HEALTH_MAX = 1.0f;

static size_t next_id = 1;
const size_t Unit::GetNextId() {
	return next_id;
}
const void Unit::SetNextId( const size_t id ) {
	next_id = id;
}

Unit::Unit( const size_t id, const Def* def, Slot* owner, map::Tile* tile, const movement_t movement, const morale_t morale, const health_t health )
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

// TODO: set these from gse
typedef const std::unordered_map< Unit::morale_t, std::string > morale_strings_t;
const morale_strings_t s_morale_strings_native = {
	{ 1, "Hatchling" },
	{ 2, "Larval Mass" },
	{ 3, "Pre-Boil" },
	{ 4, "Boil" },
	{ 5, "Mature Boil" },
	{ 6, "Great Boil" },
	{ 7, "Demon Boil" },
};
const morale_strings_t s_morale_strings_nonnative = {
	{ 1, "Very green" },
	{ 2, "Green" },
	{ 3, "Disciplined" },
	{ 4, "Hardened" },
	{ 5, "Veteran" },
	{ 6, "Commando" },
	{ 7, "Elite" },
};

const Unit::movement_t Unit::MINIMUM_MOVEMENT_TO_KEEP = 0.1f;

const bool Unit::HasMovesLeft() const {
	return m_movement >= unit::Unit::MINIMUM_MOVEMENT_TO_KEEP;
}

const Unit::movement_result_t Unit::TryMovingTo( Game* game, const map::Tile* dst_tile ) {
	// TODO: move to scripts
	ASSERT_NOLOG( m_movement >= MINIMUM_MOVEMENT_TO_KEEP, "no movement points" );

	auto movement_cost = dst_tile->GetMovementCost();
	bool move_success = false;

	if ( m_movement >= movement_cost ) {
		move_success = true;
		movement_cost += dst_tile->GetMovementAftercost();
	}
	else {
		move_success = game->GetRandom()->GetFloat( 0.0f, movement_cost ) < m_movement;
	}

	// reduce remaining movement points (even if failed)
	if ( m_movement >= movement_cost ) {
		m_movement -= movement_cost;
		if ( !HasMovesLeft() ) {
			// don't keep tiny leftovers
			m_movement = 0.0f;
		}
	}
	else {
		m_movement = 0.0f;
	}

	if ( move_success ) {
		return MR_MOVED;
	}
	else {
		return MR_TRIED_BUT_FAILED;
	}

}

const std::string& Unit::GetMoraleString( const morale_t morale ) {
	const bool is_native = true; // TODO: non-native units
	const auto& morale_strings = is_native ? s_morale_strings_native : s_morale_strings_nonnative;
	ASSERT_NOLOG( morale_strings.find( morale ) != morale_strings.end(), "unknown morale type: " + std::to_string( morale ) );
	return morale_strings.at( morale );
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
	const auto* def = Def::Unserialize( defbuf );
	auto* slot = game ? &game->GetState()->m_slots.GetSlot( buf.ReadInt() ) : nullptr;
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	auto* tile = game ? game->GetMap()->GetTile( pos_x, pos_y ) : nullptr;
	const auto movement = (movement_t)buf.ReadFloat();
	const auto morale = (morale_t)buf.ReadInt();
	const auto health = (health_t)buf.ReadFloat();
	return new Unit( id, def, slot, tile, movement, morale, health );
}

WRAPIMPL_BEGIN( Unit, CLASS_UNIT )
	WRAPIMPL_PROPS {
		{
			"id",
			VALUE( gse::type::Int, m_id )
		},
		{
			"movement",
			VALUE( gse::type::Float, m_movement )
		},
		{
			"morale",
			VALUE( gse::type::Int, m_morale )
		},
		{
			"health",
			VALUE( gse::type::Float, m_health )
		},
		{
			"get_def",
			NATIVE_CALL( this ) {
				return m_def->Wrap();
			})
		},
		{
			"get_owner",
			NATIVE_CALL( this ) {
				return m_owner->Wrap();
			})
		},
		{
			"get_tile",
			NATIVE_CALL( this ) {
				return m_tile->Wrap();
			})
		},
	};
WRAPIMPL_END_PTR( Unit )

UNWRAPIMPL_PTR( Unit )

}
}
