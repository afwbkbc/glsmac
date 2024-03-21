#include "SkipUnitTurn.h"

#include "../Game.h"

#include "gse/type/Undefined.h"

namespace game {
namespace event {

SkipUnitTurn::SkipUnitTurn( const size_t initiator_slot, const size_t unit_id )
	: Event( initiator_slot, ET_UNIT_SKIP_TURN )
	, m_unit_id( unit_id ) {
	//
}

const std::string* SkipUnitTurn::Validate( Game* game ) const {
	const auto* unit = game->GetUnit( m_unit_id );
	if ( !unit ) {
		return Error( "Unit not found" );
	}

	if ( unit->m_owner->GetIndex() != m_initiator_slot ) {
		return Error( "Unit can only be moved by it's owner" );
	}

	if ( !unit->HasMovesLeft() ) {
		return Error( "Unit is out of moves" );
	}

	return Ok();
}

const gse::Value SkipUnitTurn::Apply( game::Game* game ) const {
	game->SkipUnitTurn( m_unit_id );
	return VALUE( gse::type::Undefined );
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
