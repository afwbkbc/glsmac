#pragma once

#include "Event.h"

#include "util/CRC32.h"

namespace game {
namespace event {

class TurnFinalized : public Event {
public:
	TurnFinalized( const size_t initiator_slot, const util::CRC32::crc_t checksum );

	const std::string* Validate( Game* game ) const override;
	const gse::Value Apply( game::Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	const util::CRC32::crc_t m_checksum;

	static void Serialize( types::Buffer& buf, const TurnFinalized* event );
	static TurnFinalized* Unserialize( types::Buffer& buf, const size_t initiator_slot );

};

}
}
