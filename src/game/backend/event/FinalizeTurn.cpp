#include "FinalizeTurn.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"

#include "gse/type/Undefined.h"

namespace game {
namespace backend {
namespace event {

FinalizeTurn::FinalizeTurn( const size_t initiator_slot )
	: Event( initiator_slot, ET_FINALIZE_TURN ) {
	//
}

const std::string* FinalizeTurn::Validate( Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Turn can only be finalized by master" );
	}
	if ( !game->IsTurnActive() ) {
		return Error( "Tried to finalize but turn is not active" );
	}
	return Ok();
}

const gse::Value FinalizeTurn::Apply( Game* game ) const {
	game->FinalizeTurn();
	return VALUE( gse::type::Undefined );
}

TS_BEGIN( FinalizeTurn )
		TS_FUNC_BEGIN( "FinalizeTurn" ) +
		TS_FUNC_END()
TS_END()

void FinalizeTurn::Serialize( types::Buffer& buf, const FinalizeTurn* event ) {
}

FinalizeTurn* FinalizeTurn::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	return new FinalizeTurn( initiator_slot );
}

}
}
}
