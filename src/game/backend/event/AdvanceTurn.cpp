#include "AdvanceTurn.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"

#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

AdvanceTurn::AdvanceTurn( const size_t initiator_slot, const size_t turn_id )
	: LegacyEvent( initiator_slot, ET_ADVANCE_TURN )
	, m_turn_id( turn_id ) {
	//
}

const std::string* AdvanceTurn::Validate( GSE_CALLABLE, Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Turn can only be advanced by master" );
	}
	return Ok();
}

gse::Value* const AdvanceTurn::Apply( GSE_CALLABLE, Game* game ) const {
	game->AdvanceTurn( m_turn_id );
	return VALUEEXT( gse::value::Undefined, game->GetGCSpace() );
}

TS_BEGIN( AdvanceTurn )
		TS_FUNC_BEGIN( "AdvanceTurn" ) +
			TS_FUNC_ARG_NUM( "turn_id", m_turn_id ) +
		TS_FUNC_END()
TS_END()

void AdvanceTurn::Serialize( types::Buffer& buf, const AdvanceTurn* event ) {
	buf.WriteInt( event->m_turn_id );
}

AdvanceTurn* AdvanceTurn::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const size_t turn_id = buf.ReadInt();
	return new AdvanceTurn( initiator_slot, turn_id );
}

}
}
}
