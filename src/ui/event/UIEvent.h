#pragma once

#include "common/Common.h"

#include "Types.h"

namespace ui {
namespace event {

CLASS( UIEvent, common::Class )

	const event_type_t m_type = EV_NONE;
	event_flag_t m_flags = EF_NONE;
	event_data_t m_data = {};

	// event will stop propagating after this
	void SetProcessed();
	const bool IsProcessed() const;

	// needed to avoid mouseover duplications on objects underneath front one
	void SetMouseOverHappened();
	const bool IsMouseOverHappened();

	// copy from other event
	UIEvent( const UIEvent* other );
	UIEvent( const event_type_t type, const event_data_t* data );

protected:
	UIEvent( const event_type_t type );

private:
	bool m_is_processed = false;
	bool m_is_mouseover_happened = false;

};

}
}
