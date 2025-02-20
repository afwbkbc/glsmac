#include "AttackUnit.h"

#include "game/backend/Game.h"
#include "game/backend/unit/UnitManager.h"
#include "game/backend/unit/StaticDef.h"
#include "game/backend/unit/Unit.h"
#include "game/backend/slot/Slot.h"
#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

AttackUnit::AttackUnit( const size_t initiator_slot, const size_t attacker_unit_id, const size_t defender_unit_id )
	: Event( initiator_slot, ET_UNIT_ATTACK )
	, m_attacker_unit_id( attacker_unit_id )
	, m_defender_unit_id( defender_unit_id ) {}

const std::string* AttackUnit::Validate( Game* game ) const {
	auto* attacker = game->GetUM()->GetUnit( m_attacker_unit_id );
	if ( !attacker ) {
		return Error( "Attacker unit not found" );
	}
	if ( attacker->m_owner->GetIndex() != m_initiator_slot ) {
		return Error( "Unit can only be ordered to attack by it's owner" );
	}
	if ( attacker->GetTile()->IsLocked() ) {
		return Error( "Attacker tile is locked" );
	}
	auto* defender = game->GetUM()->GetUnit( m_defender_unit_id );
	if ( !defender ) {
		return Error( "Defender unit not found" );
	}
	if ( defender->GetTile()->IsLocked() ) {
		return Error( "Attacker tile is locked" );
	}
	return game->GetUM()->AttackUnitValidate( attacker, defender );
}

void AttackUnit::Resolve( Game* game ) {
	auto* attacker = game->GetUM()->GetUnit( m_attacker_unit_id );
	ASSERT_NOLOG( attacker, "attacker unit not found" );
	auto* defender = game->GetUM()->GetUnit( m_defender_unit_id );
	ASSERT_NOLOG( defender, "defender unit not found" );

	m_resolutions = game->GetUM()->AttackUnitResolve( attacker, defender );
}

gse::Value* const AttackUnit::Apply( Game* game ) const {
	auto* attacker = game->GetUM()->GetUnit( m_attacker_unit_id );
	ASSERT_NOLOG( attacker, "attacker unit not found" );
	auto* defender = game->GetUM()->GetUnit( m_defender_unit_id );
	ASSERT_NOLOG( defender, "defender unit not found" );
	ASSERT_NOLOG( m_resolutions, "resolutions is null" );
	game->GetUM()->AttackUnitApply( attacker, defender, m_resolutions );
	return VALUEEXT( gse::value::Undefined, game->GetGCSpace() );
}

TS_BEGIN( AttackUnit )
		TS_FUNC_BEGIN( "AttackUnit" ) +
			TS_FUNC_ARG_NUM( "attacker_unit_id", m_attacker_unit_id ) +
			TS_FUNC_ARG_NUM( "defender_unit_id", m_defender_unit_id ) +
		TS_FUNC_END()
TS_END()

void AttackUnit::Serialize( types::Buffer& buf, const AttackUnit* event ) {
	buf.WriteInt( event->m_attacker_unit_id );
	buf.WriteInt( event->m_defender_unit_id );
	types::Buffer b = {};
	gse::Value::Serialize( &b, event->m_resolutions );
	buf.WriteString( b.ToString() );
}

AttackUnit* AttackUnit::Unserialize( gc::Space* const gc_space, types::Buffer& buf, const size_t initiator_slot ) {
	const auto attacker_unit_id = buf.ReadInt();
	const auto defender_unit_id = buf.ReadInt();
	auto* result = new AttackUnit( initiator_slot, attacker_unit_id, defender_unit_id );
	types::Buffer b( buf.ReadString() );
	result->m_resolutions = gse::Value::Unserialize( gc_space, &b );
	return result;
}

}
}
}
