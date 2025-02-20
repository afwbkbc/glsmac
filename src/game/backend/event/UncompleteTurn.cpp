#include "UncompleteTurn.h"

#include "game/backend/Game.h"

#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

UncompleteTurn::UncompleteTurn( const size_t initiator_slot, const size_t turn_id )
	: Event( initiator_slot, ET_UNCOMPLETE_TURN )
	, m_turn_id( turn_id ) {
	//
}

const std::string* UncompleteTurn::Validate( Game* game ) const {
	if ( game->GetTurnId() != m_turn_id ) {
		return Error( "Turn ID mismatch" );
	}
	if ( !game->IsTurnCompleted( m_initiator_slot ) ) {
		return Error( "Turn not completed" );
	}
	return Ok();
}

gse::Value* const UncompleteTurn::Apply( Game* game ) const {
	game->UncompleteTurn( m_initiator_slot );
	return VALUEEXT( gse::value::Undefined, game->GetGCSpace() );
}

TS_BEGIN( UncompleteTurn )
		TS_FUNC_BEGIN( "UncompleteTurn" ) +
			TS_FUNC_ARG_NUM( "turn_id", m_turn_id ) +
		TS_FUNC_END()
TS_END()

void UncompleteTurn::Serialize( types::Buffer& buf, const UncompleteTurn* event ) {
	buf.WriteInt( event->m_turn_id );
}

UncompleteTurn* UncompleteTurn::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const size_t turn_id = buf.ReadInt();
	return new UncompleteTurn( initiator_slot, turn_id );
}

}
}
}
