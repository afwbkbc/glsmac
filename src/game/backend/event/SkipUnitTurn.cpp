#include "SkipUnitTurn.h"

#include "game/backend/Game.h"
#include "game/backend/unit/UnitManager.h"
#include "game/backend/unit/Unit.h"
#include "game/backend/slot/Slot.h"
#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

SkipUnitTurn::SkipUnitTurn( const size_t initiator_slot, const size_t unit_id )
	: LegacyEvent( initiator_slot, ET_UNIT_SKIP_TURN )
	, m_unit_id( unit_id ) {
	//
}

const std::string* SkipUnitTurn::Validate( GSE_CALLABLE, Game* game ) const {
	const auto* unit = game->GetUM()->GetUnit( m_unit_id );
	if ( !unit ) {
		return Error( "Unit not found" );
	}

	if ( unit->GetTile()->IsLocked() ) {
		return Error( "Unit tile is locked" );
	}

	if ( unit->m_owner->GetIndex() != m_initiator_slot ) {
		return Error( "Unit can only be moved by it's owner" );
	}

	if ( !unit->HasMovesLeft() ) {
		return Error( "Unit is out of moves" );
	}

	return Ok();
}

gse::Value* const SkipUnitTurn::Apply( GSE_CALLABLE, Game* game ) const {
	game->GetUM()->SkipUnitTurn( GSE_CALL, m_unit_id );
	return VALUEEXT( gse::value::Undefined, game->GetGCSpace() );
}

TS_BEGIN( SkipUnitTurn )
		TS_FUNC_BEGIN( "SkipUnitTurn" ) +
			TS_FUNC_ARG_NUM( "unit_id", m_unit_id ) +
		TS_FUNC_END()
TS_END()

void SkipUnitTurn::Serialize( types::Buffer& buf, const SkipUnitTurn* event ) {
	buf.WriteInt( event->m_unit_id );
}

SkipUnitTurn* SkipUnitTurn::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const auto unit_id = buf.ReadInt();
	return new SkipUnitTurn( initiator_slot, unit_id );
}

}
}
}
