#pragma once

// TODO: combine with Button

#include "UIContainer.h"
#include "Surface.h"

#include "util/Timer.h"

namespace ui {
namespace object {

CLASS( SimpleButton, UIContainer )

	SimpleButton( const std::string& class_name = "" );

	void Create() override;
	void Destroy() override;

protected:

	bool OnMouseOver( const UIEvent::event_data_t* data ) override;
	bool OnMouseOut( const UIEvent::event_data_t* data ) override;
	bool OnMouseDown( const UIEvent::event_data_t* data ) override;
	bool OnMouseUp( const UIEvent::event_data_t* data ) override;

	bool m_is_clicking = false; // mouseup at different position after mousedown is still counted as click, as long as it's inside button

	Surface* m_background = nullptr;

private:
	bool m_maybe_doubleclick = false;
	util::Timer m_doubleclick_timer;

};

} /* namespace object */
} /* namespace ui */
