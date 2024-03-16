#include "CompleteTurn.h"

#include "../Game.h"
#include "../State.h"

#include "gse/type/Undefined.h"

namespace game {
namespace event {

CompleteTurn::CompleteTurn( const size_t initiator_slot )
	: Event( initiator_slot, ET_COMPLETE_TURN ) {
	//
}

const std::string* CompleteTurn::Validate( const Game* game ) const {
	if ( game->IsTurnCompleted( m_initiator_slot ) ) {
		return Error( "Turn already completed" );
	}
	return Ok();
}

const gse::Value CompleteTurn::Apply( game::Game* game ) const {
	game->CompleteTurn( m_initiator_slot );
	return VALUE( gse::type::Undefined );
}

TS_BEGIN( CompleteTurn )
		TS_FUNC_BEGIN( "CompleteTurn" ) +
		TS_FUNC_END()
TS_END()

void CompleteTurn::Serialize( types::Buffer& buf, const CompleteTurn* event ) {
}

CompleteTurn* CompleteTurn::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	return new CompleteTurn( initiator_slot );
}

}
}
