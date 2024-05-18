#include "AdvanceTurn.h"

#include "../Game.h"
#include "../State.h"

#include "gse/type/Undefined.h"

namespace game {
namespace event {

AdvanceTurn::AdvanceTurn( const size_t initiator_slot, const size_t turn_id )
	: Event( initiator_slot, ET_ADVANCE_TURN )
	, m_turn_id( turn_id ) {
	//
}

const std::string* AdvanceTurn::Validate( Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Turn can only be advanced by master" );
	}
	return Ok();
}

const gse::Value AdvanceTurn::Apply( game::Game* game ) const {
	game->AdvanceTurn( m_turn_id );
	return VALUE( gse::type::Undefined );
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
