#pragma once

#include <vector>

#include "base/Base.h"

#include "game/world/event/Event.h"

namespace game {
namespace world {

CLASS( Turn, base::Base )
	typedef std::vector< const world::event::Event* > events_t;
	Turn( const size_t year );
	const size_t GetYear() const;
	void AddEvent( const world::event::Event* event );
	const events_t* const GetEvents() const;
private:
	const size_t m_year;
	events_t m_events = {};
};

}
}

