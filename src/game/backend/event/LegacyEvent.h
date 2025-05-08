#pragma once

#include <vector>

#include "types/Buffer.h"

namespace game {
namespace backend {

class Game;

namespace event {

class LegacyEvent {
public:
	enum event_type_t {
		ET_NONE,
		ET_FINALIZE_TURN,
		ET_TURN_FINALIZED,
		ET_ADVANCE_TURN,
	};

	LegacyEvent( const size_t initiator_slot, const event_type_t type );
	virtual ~LegacyEvent() = default;

	static const types::Buffer Serialize( const LegacyEvent* event );
	static LegacyEvent* Unserialize( GSE_CALLABLE, types::Buffer& buf );

	static const types::Buffer SerializeMultiple( const std::vector< LegacyEvent* >& events );
	static void UnserializeMultiple( GSE_CALLABLE, types::Buffer& buf, std::vector< LegacyEvent* >& events_out );

	const event_type_t m_type;
	const size_t m_initiator_slot;

	void SetDestinationSlot( const uint8_t destination_slot );
	const bool IsProcessableBy( const uint8_t destination_slot ) const;
	const bool IsSendableTo( const uint8_t destination_slot ) const;

	virtual const std::string* Validate( GSE_CALLABLE, Game* game ) const = 0;
	virtual void Resolve( GSE_CALLABLE, Game* game ) {};
	virtual gse::Value* const Apply( GSE_CALLABLE, Game* game ) const = 0;
	virtual const std::string ToString( const std::string& prefix = "" ) const = 0;

	bool m_is_validated = false;

protected:
	const std::string* Ok() const;
	const std::string* Error( const std::string& text ) const;

private:
	bool m_is_public = true;
	uint8_t m_destination_slot = 0;
};

}
}
}
