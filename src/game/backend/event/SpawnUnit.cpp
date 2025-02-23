#include "SpawnUnit.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/map/Map.h"
#include "game/backend/unit/UnitManager.h"
#include "game/backend/unit/StaticDef.h"
#include "game/backend/unit/Unit.h"

namespace game {
namespace backend {
namespace event {

SpawnUnit::SpawnUnit(
	const size_t initiator_slot,
	const std::string& unit_def,
	const size_t owner_slot,
	const size_t pos_x,
	const size_t pos_y,
	const unit::morale_t morale,
	const unit::health_t health
)
	: Event( initiator_slot, ET_UNIT_SPAWN )
	, m_unit_def( unit_def )
	, m_owner_slot( owner_slot )
	, m_pos_x( pos_x )
	, m_pos_y( pos_y )
	, m_morale( morale )
	, m_health( health ) {
	//
}

const std::string* SpawnUnit::Validate( GSE_CALLABLE, Game* game ) const {
	if ( !game->GetUM() ) {
		return Error( "Game is not initialized properly" );
	}
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to spawn units" );
	}
	return Ok();
}

gse::Value* const SpawnUnit::Apply( GSE_CALLABLE, Game* game ) const {
	auto* um = game->GetUM();
	ASSERT_NOLOG( um, "um is null" );
	auto* def = um->GetUnitDef( m_unit_def );
	ASSERT_NOLOG( def, "unit def '" + m_unit_def + "' not found" );
	ASSERT_NOLOG( def->m_type == unit::DT_STATIC, "only static defs are supported" );
	const auto* staticdef = (unit::StaticDef*)def;
	auto& owner = game->GetState()->m_slots->GetSlot( m_owner_slot );
	auto* tile = game->GetMap()->GetTile( m_pos_x, m_pos_y );
	auto* unit = new unit::Unit(
		GSE_CALL,
		um,
		unit::Unit::GetNextId(),
		def,
		&owner,
		tile,
		staticdef->m_movement_per_turn,
		m_morale,
		m_health,
		false
	);
	game->GetUM()->SpawnUnit( GSE_CALL, unit );
	return unit->Wrap( GSE_CALL );
}

TS_BEGIN( SpawnUnit )
		TS_FUNC_BEGIN( "SpawnUnit" ) +
			TS_FUNC_ARG_STR( "def", m_unit_def ) +
			TS_FUNC_ARG_NUM( "owner_slot", m_owner_slot ) +
			TS_FUNC_ARG_NUM( "pos_x", m_pos_x ) +
			TS_FUNC_ARG_NUM( "pos_y", m_pos_y ) +
			TS_FUNC_ARG_NUM( "morale", m_morale ) +
			TS_FUNC_ARG_NUM( "health", m_health ) +
		TS_FUNC_END()
TS_END()

void SpawnUnit::Serialize( types::Buffer& buf, const SpawnUnit* event ) {
	buf.WriteString( event->m_unit_def );
	buf.WriteInt( event->m_owner_slot );
	buf.WriteInt( event->m_pos_x );
	buf.WriteInt( event->m_pos_y );
	buf.WriteInt( event->m_morale );
	buf.WriteFloat( event->m_health );
}

SpawnUnit* SpawnUnit::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const auto unit_def = buf.ReadString();
	const auto owner_slot = buf.ReadInt();
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	const auto morale = (unit::morale_t)buf.ReadInt();
	const auto health = (unit::health_t)buf.ReadFloat();
	return new SpawnUnit( initiator_slot, unit_def, owner_slot, pos_x, pos_y, morale, health );
}

}
}
}
