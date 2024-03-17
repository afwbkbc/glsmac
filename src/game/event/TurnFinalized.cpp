#include "TurnFinalized.h"

#include "../Game.h"
#include "../State.h"

#include "gse/type/Undefined.h"

namespace game {
namespace event {

TurnFinalized::TurnFinalized( const size_t initiator_slot, const util::CRC32::crc_t checksum )
	: Event( initiator_slot, ET_TURN_FINALIZED )
	, m_checksum( checksum ) {
	//
}

const std::string* TurnFinalized::Validate( const Game* game ) const {
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

const gse::Value TurnFinalized::Apply( game::Game* game ) const {
	if ( game->GetState()->IsMaster() ) {
		game->GlobalProcessTurnFinalized( m_initiator_slot, m_checksum );
	}
	return VALUE( gse::type::Undefined );
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
	const util::CRC32::crc_t checksum = buf.ReadInt();
	return new TurnFinalized( initiator_slot, checksum );
}

}
}
