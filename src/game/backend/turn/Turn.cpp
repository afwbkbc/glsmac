#include "Turn.h"

namespace game {
namespace backend {
namespace turn {

Turn::Turn() {
	//
}

Turn::~Turn() {
	Reset();
}

const size_t Turn::GetId() const {
	return m_id;
}

void Turn::AdvanceTurn( const size_t turn_id ) {
	m_id = turn_id;
}

const util::crc32::crc_t Turn::FinalizeAndChecksum() {
	THROW( "TODO: FINALIZE_AND_CHECKSUM" );
	//return util::crc32::CRC32::CalculateFromBuffer( event::Event::SerializeMultiple( m_events ) );
}

void Turn::Reset() {
	m_id = 0;
}

}
}
}
