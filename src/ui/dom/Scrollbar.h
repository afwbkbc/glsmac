#pragma once

#include "Panel.h"

#include "util/Timer.h"

namespace ui {
namespace dom {

class Button;

class Scrollbar : public Panel {
public:
	Scrollbar( DOM_ARGS_TD( "scrollbar" ) );

protected:
	virtual const bool ProcessEvent( GSE_CALLABLE, const input::Event& event ) override;

private:

	enum scroll_type_t {
		ST_VERTICAL,
		ST_HORIZONTAL,
	};
	scroll_type_t m_scroll_type = ST_VERTICAL;

	float m_min = 0.0f;
	float m_max = 100.0f;
	float m_value = 0.0f;
	float m_speed = 1.0f;

	Button* m_from; // top or left
	Button* m_to; // bottom or right
	Button* m_slider;

	size_t m_fromto_size = 0;
	size_t m_slider_size = 0;

	struct {
		const size_t frequency_ms = 10;
		util::Timer timer;
		float direction = 0.0f;
	} m_arrow_scroll;

	struct {
		bool is_dragging = false;
		int initial_offset = 0;
	} m_slider_drag;

	void SetMin( GSE_CALLABLE, const float min );
	void SetMax( GSE_CALLABLE, const float max );
	void SetValue( GSE_CALLABLE, float value, const bool send_event = false );
	void SetSpeed( GSE_CALLABLE, float speed );
	void Resize();
	void ResizeFromTo();
	void ResizeSlider();
	void RealignSlider();
	void Scroll();
};

}
}
