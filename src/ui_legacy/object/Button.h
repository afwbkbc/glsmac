#pragma once

// TODO: combine with SimpleButton

#include "Panel.h"

#include "util/Timer.h"

namespace ui_legacy {
namespace object {

class Label;
class SoundEffect;

CLASS( Button, Panel )

	Button( const std::string& class_name = "" );

	virtual void Create() override;
	virtual void Destroy() override;

	void SetLabel( const std::string& text );
	const std::string& GetLabel() const;
	void SetTextAlign( alignment_t alignment );

protected:
	virtual void ApplyStyle() override;

	bool OnMouseOver( const event::event_data_t* data ) override;
	bool OnMouseOut( const event::event_data_t* data ) override;
	bool OnMouseDown( const event::event_data_t* data ) override;
	bool OnMouseUp( const event::event_data_t* data ) override;

	std::string m_label_text = "";
	alignment_t m_label_alignment = ALIGN_CENTER;
	Label* m_label = nullptr;

	bool m_is_clicking = false; // mouseup at different position after mousedown is still counted as click, as long as it's inside button

private:
	bool m_maybe_doubleclick = false;
	util::Timer m_doubleclick_timer;

	SoundEffect* m_click_sound = nullptr;
	SoundEffect* m_move_sound = nullptr;
};

}
}
