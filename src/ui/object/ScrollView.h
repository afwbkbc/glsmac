#pragma once

#include <vector>

#include "Panel.h"

#include "util/Scroller.h"

#include "Scrollbar.h"

namespace ui {
namespace object {

CLASS( ScrollView, Panel )

	struct consts_t {
		const struct {
			const uint16_t scroll_time_ms = 100;
			const uint16_t scroll_step_ms = 2;
			const uint8_t scroll_steps = scroll_time_ms / scroll_step_ms;
		} smooth_scrolling;
	};

	ScrollView( const std::string& class_name = "" )
		: Panel( class_name ) {}

	virtual void Create() override;
	virtual void Iterate() override;
	virtual void Destroy() override;

	void AddChild( UIObject* object );
	void RemoveChild( UIObject* object );

	void SetWidth( const coord_t px ) override;
	void SetHeight( const coord_t px ) override;

	void SetScroll( vertex_t px, const bool force = false );
	void SetScrollX( const coord_t px );
	void SetScrollY( const coord_t px );
	void ScrollToBottom();

	void SetScrollSpeed( const size_t scroll_speed );

	void SetInternalWidth( const coord_t px );
	void SetInternalHeight( const coord_t px );

protected:
	void ScrollToObjectMaybe( UIObject* object );

private:

	size_t m_scroll_speed = 17;
	bool m_is_sticky = true;

	Scrollbar* m_scrollbar = nullptr;

	vertex_t m_internal_size = {
		0,
		0
	};
	vertex_t m_scroll = {
		0,
		0
	};
	void UpdateInternalSize();

	const coord_box_t GetScrollLimits();
	void ScrollTo( float target );

	std::vector< UIObject* > m_to_add = {};
	std::vector< UIObject* > m_to_remove = {};

	util::Scroller< float > m_scroller;
	bool m_need_stickyness = false;

	UIContainer* m_viewport = nullptr;
	Panel* m_body = nullptr;

	bool m_need_body_refresh = false;

	bool m_is_dragging = false;
	Vec2< ssize_t > m_drag_start_position = {
		0,
		0
	};
	struct {
		const UIEventHandler* mouse_up;
		const UIEventHandler* mouse_move;
	} m_handlers;

	void AddChildToBody( UIObject* child );
	void RemoveChildFromBody( UIObject* child );
	void FixScrollY();

};

}
}
