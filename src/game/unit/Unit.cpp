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

Unit::Unit( const size_t id, const Def* def, Slot* owner, map::Tile* tile, const morale_t morale, const health_t health )
	: m_id( id )
	, m_def( def )
	, m_owner( owner )
	, m_tile( tile )
	, m_morale( morale )
	, m_health( health ) {
	if ( next_id <= id ) {
		next_id = id + 1;
	}
}

typedef const std::unordered_map< Unit::morale_t, std::string > morale_strings_t;
const morale_strings_t s_morale_strings_native = {
/*	{ Unit::M_VERYGREEN, "Hatchling" },
	{ Unit::M_GREEN, "Larval Mass" },
	{ Unit::M_DISCIPLINED, "Pre-Boil" },
	{ Unit::M_HARDENED, "Boil" },
	{ Unit::M_VETERAN, "Mature Boil" },
	{ Unit::M_COMMANDO, "Great Boil" },
	{ Unit::M_ELITE, "Demon Boil" },*/
};
const morale_strings_t s_morale_strings_nonnative = {
/*	{ Unit::M_VERYGREEN, "Very green" },
	{ Unit::M_GREEN, "Green" },
	{ Unit::M_DISCIPLINED, "Disciplined" },
	{ Unit::M_HARDENED, "Hardened" },
	{ Unit::M_VETERAN, "Veteran" },
	{ Unit::M_COMMANDO, "Commando" },
	{ Unit::M_ELITE, "Elite" },*/
};
const std::string& Unit::GetMoraleString() const {
	const bool is_native = true; // TODO: non-native units
	const auto& morale_strings = is_native ? s_morale_strings_native : s_morale_strings_nonnative;
	ASSERT_NOLOG( morale_strings.find( m_morale ) != morale_strings.end(), "unknown morale type: " + std::to_string( m_morale ) );
	return morale_strings.at( m_morale );
}

const types::Buffer Unit::Serialize( const Unit* unit ) {
	types::Buffer buf;
	buf.WriteInt( unit->m_id );
	buf.WriteString( Def::Serialize( unit->m_def ).ToString() );
	buf.WriteInt( unit->m_owner->GetIndex() );
	buf.WriteInt( unit->m_tile->coord.x );
	buf.WriteInt( unit->m_tile->coord.y );
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
	const auto morale = (morale_t)buf.ReadInt();
	const auto health = (health_t)buf.ReadFloat();
	return new Unit( id, def, slot, tile, morale, health );
}

WRAPIMPL_BEGIN( Unit, CLASS_UNIT )
	WRAPIMPL_PROPS {
		{
			"id",
			VALUE( gse::type::Int, m_id )
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
