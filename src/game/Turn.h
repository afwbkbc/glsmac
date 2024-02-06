#pragma once

#include <vector>

#include "base/Base.h"

#include "event/Event.h"

namespace game {

CLASS( Turn, base::Base )
	typedef std::vector< const event::Event* > events_t;
	Turn( const size_t id );
	~Turn();
	const size_t GetId() const;
	void AddEvent( const event::Event* event );
	const events_t* const GetEvents() const;
private:
	const size_t m_id;
	events_t m_events = {};
};

}
