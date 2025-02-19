#include "CompleteTurn.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"

#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

CompleteTurn::CompleteTurn( const size_t initiator_slot, const size_t turn_id )
	: Event( initiator_slot, ET_COMPLETE_TURN )
	, m_turn_id( turn_id ) {
	//
}

const std::string* CompleteTurn::Validate( Game* game ) const {
	if ( game->GetTurnId() != m_turn_id ) {
		return Error( "Turn ID mismatch ( " + std::to_string( game->GetTurnId() ) + " != " + std::to_string( m_turn_id ) + " )" );
	}
	if ( game->IsTurnCompleted( m_initiator_slot ) ) {
		return Error( "Turn already completed" );
	}
	return Ok();
}

gse::Value* const CompleteTurn::Apply( Game* game ) const {
	game->CompleteTurn( m_initiator_slot );
	return VALUE( gse::value::Undefined );
}

TS_BEGIN( CompleteTurn )
		TS_FUNC_BEGIN( "CompleteTurn" ) +
			TS_FUNC_ARG_NUM( "turn_id", m_turn_id ) +
		TS_FUNC_END()
TS_END()

void CompleteTurn::Serialize( types::Buffer& buf, const CompleteTurn* event ) {
	buf.WriteInt( event->m_turn_id );
}

CompleteTurn* CompleteTurn::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const size_t turn_id = buf.ReadInt();
	return new CompleteTurn( initiator_slot, turn_id );
}

}
}
}
