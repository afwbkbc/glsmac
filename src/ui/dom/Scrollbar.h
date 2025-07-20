#pragma once

#include "Panel.h"

#include <functional>

#include "util/Timer.h"
#include "util/Scroller.h"

namespace ui {
namespace dom {

class Button;

class Scrollbar : public Panel {
public:
	Scrollbar( DOM_ARGS_TD( "scrollbar" ) );

	void Destroy( GSE_CALLABLE ) override;

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
		const float speed = 2.0f;
	} m_arrow_scroll;

	struct {
		bool is_dragging = false;
		int initial_offset = 0;
		size_t drag_handler_id = 0;
	} m_slider_drag;

	struct {
		const size_t duration_ms = 200;
		const float speed = 1.0f;
		util::Scroller< float > scroller;
	} m_wheel_scroll;

	void SetMin( GSE_CALLABLE, const float min );
	void SetMax( GSE_CALLABLE, const float max );
	void SetValue( GSE_CALLABLE, float value, const bool send_event = false );
	void SetSpeed( GSE_CALLABLE, float speed );
	void Resize();
	void ResizeFromTo();
	void ResizeSlider();
	void RealignSlider();
	void Scroll( const float value );

private:
	friend class Scrollview;
	friend class Listview;
	std::function< const bool( const float value ) > m_on_change = nullptr;
	void SetMinRaw( const float min );
	void SetMaxRaw( const float max );
	void SetValueRaw( float value, const bool send_event = false );
	void SetScrollType( const scroll_type_t type );
};

}
}
