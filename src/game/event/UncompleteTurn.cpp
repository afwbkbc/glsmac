#include "UncompleteTurn.h"

#include "../Game.h"

#include "gse/type/Undefined.h"

namespace game {
namespace event {

UncompleteTurn::UncompleteTurn( const size_t initiator_slot, const size_t turn_id )
	: Event( initiator_slot, ET_UNCOMPLETE_TURN )
	, m_turn_id( turn_id ) {
	//
}

const std::string* UncompleteTurn::Validate( const Game* game ) const {
	if ( game->GetTurnId() != m_turn_id ) {
		return Error( "Turn ID mismatch" );
	}
	if ( !game->IsTurnCompleted( m_initiator_slot ) ) {
		return Error( "Turn not completed" );
	}
	return Ok();
}

const gse::Value UncompleteTurn::Apply( game::Game* game ) const {
	game->UncompleteTurn( m_initiator_slot );
	return VALUE( gse::type::Undefined );
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
