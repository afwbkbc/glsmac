#pragma once

// TODO: combine with Button

#include "Surface.h"

#include "util/Timer.h"

namespace ui {
namespace object {

CLASS( SimpleButton, Surface )

	SimpleButton( const std::string& class_name = "" );

protected:
	
	bool OnMouseOver( const UIEvent::event_data_t* data );
	bool OnMouseOut( const UIEvent::event_data_t* data );
	bool OnMouseDown( const UIEvent::event_data_t* data );
	bool OnMouseUp( const UIEvent::event_data_t* data );
	
	bool m_is_clicking = false; // mouseup at different position after mousedown is still counted as click, as long as it's inside button
	
private:
	bool m_maybe_doubleclick = false;
	util::Timer m_doubleclick_timer;

};

} /* namespace object */
} /* namespace ui */
