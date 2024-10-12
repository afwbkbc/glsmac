#pragma once

// TODO: combine with Button

#include "UIContainer.h"

#include "util/Timer.h"

namespace ui {
namespace object {

class Surface;
class SoundEffect;

CLASS( SimpleButton, UIContainer )

	SimpleButton( const std::string& class_name = "" );

	void Create() override;
	void Destroy() override;

protected:

	bool OnMouseOver( const event::event_data_t* data ) override;
	bool OnMouseOut( const event::event_data_t* data ) override;
	bool OnMouseDown( const event::event_data_t* data ) override;
	bool OnMouseUp( const event::event_data_t* data ) override;

	bool m_is_clicking = false; // mouseup at different position after mousedown is still counted as click, as long as it's inside button

	Surface* m_background = nullptr;
	SoundEffect* m_click_sound = nullptr;

private:
	bool m_maybe_doubleclick = false;
	util::Timer m_doubleclick_timer;

};

}
}
