#pragma once

#include <vector>

#include "event/Event.h"
#include "util/CRC32.h"

namespace game {

class Turn {
public:
	typedef std::vector< event::Event* > events_t;

	enum turn_status_t {
		TS_NONE,
		TS_PLEASE_WAIT,
		TS_TURN_ACTIVE,
		TS_TURN_COMPLETE,
		TS_WAITING_FOR_PLAYERS,
	};

	Turn();
	~Turn();

	const bool IsActive() const;
	const size_t GetId() const;

	void AdvanceTurn( const size_t turn_id );
	const util::CRC32::crc_t FinalizeAndChecksum();

	void AddEvent( event::Event* event );
	const events_t* const GetEvents() const;
private:
	size_t m_id = 0;
	bool m_is_active = false;
	events_t m_events = {};
};

}
