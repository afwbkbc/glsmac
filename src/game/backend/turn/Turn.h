#pragma once

#include <vector>

#include "game/backend/event/LegacyEvent.h"
#include "util/crc32/CRC32.h"

namespace game {
namespace backend {
namespace turn {

class Turn {
public:
	typedef std::vector< event::LegacyEvent* > events_t;

	Turn();
	~Turn();

	const bool IsActive() const;
	const size_t GetId() const;

	void AdvanceTurn( const size_t turn_id );
	const util::crc32::crc_t FinalizeAndChecksum();

	void AddEvent( event::LegacyEvent* event );
	const events_t* const GetEvents() const;

	void Reset();

private:
	size_t m_id = 0;
	bool m_is_active = false;
	events_t m_events = {};
};

}
}
}
