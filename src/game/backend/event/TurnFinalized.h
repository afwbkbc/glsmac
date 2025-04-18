#pragma once

#include "LegacyEvent.h"

#include "util/crc32/Types.h"

namespace game {
namespace backend {
namespace event {

class TurnFinalized : public LegacyEvent {
public:
	TurnFinalized( const size_t initiator_slot, const util::crc32::crc_t checksum );

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class LegacyEvent;

	const util::crc32::crc_t m_checksum;

	static void Serialize( types::Buffer& buf, const TurnFinalized* event );
	static TurnFinalized* Unserialize( types::Buffer& buf, const size_t initiator_slot );

};

}
}
}
