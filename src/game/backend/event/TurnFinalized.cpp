#include "TurnFinalized.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"

#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

TurnFinalized::TurnFinalized( const size_t initiator_slot, const util::crc32::crc_t checksum )
	: LegacyEvent( initiator_slot, ET_TURN_FINALIZED )
	, m_checksum( checksum ) {
	//
}

const std::string* TurnFinalized::Validate( GSE_CALLABLE, Game* game ) const {
	if ( game->GetState()->IsMaster() ) {
		if ( game->IsTurnActive() ) {
			return Error( "Got finalize response but turn is still active" );
		}
		if ( !game->IsTurnChecksumValid( m_checksum ) ) {
			return Error( "Invalid turn checksum" );
		}
	}
	return Ok();
}

gse::Value* const TurnFinalized::Apply( GSE_CALLABLE, Game* game ) const {
	if ( game->GetState()->IsMaster() ) {
		game->GlobalProcessTurnFinalized( GSE_CALL, m_initiator_slot, m_checksum );
	}
	return VALUEEXT( gse::value::Undefined, game->GetGCSpace() );
}

TS_BEGIN( TurnFinalized )
		TS_FUNC_BEGIN( "TurnFinalized" ) +
			TS_FUNC_ARG_NUM( "checksum", m_checksum ) +
		TS_FUNC_END()
TS_END()

void TurnFinalized::Serialize( types::Buffer& buf, const TurnFinalized* event ) {
	buf.WriteInt( event->m_checksum );
}

TurnFinalized* TurnFinalized::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const util::crc32::crc_t checksum = buf.ReadInt();
	return new TurnFinalized( initiator_slot, checksum );
}

}
}
}
