#pragma once

#include <vector>

#include "event/Event.h"

namespace game {

class Turn {
public:
	typedef std::vector< const event::Event* > events_t;

	Turn();
	~Turn();

	const bool IsActive() const;

	void Activate();
	void Deactivate();

	void AddEvent( const event::Event* event );
	const events_t* const GetEvents() const;
private:
	bool m_is_active = false;
	events_t m_events = {};
};

}
