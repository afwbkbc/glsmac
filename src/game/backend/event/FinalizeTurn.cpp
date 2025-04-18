#include "FinalizeTurn.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"

#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

FinalizeTurn::FinalizeTurn( const size_t initiator_slot )
	: LegacyEvent( initiator_slot, ET_FINALIZE_TURN ) {
	//
}

const std::string* FinalizeTurn::Validate( GSE_CALLABLE, Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Turn can only be finalized by master" );
	}
	if ( !game->IsTurnActive() ) {
		return Error( "Tried to finalize but turn is not active" );
	}
	return Ok();
}

gse::Value* const FinalizeTurn::Apply( GSE_CALLABLE, Game* game ) const {
	game->FinalizeTurn( GSE_CALL );
	return VALUEEXT( gse::value::Undefined, game->GetGCSpace() );
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
