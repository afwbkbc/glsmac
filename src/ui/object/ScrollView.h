#pragma once

#include <vector>

#include "Panel.h"

#include "util/Timer.h"

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
	
	ScrollView( const std::string& class_name = "" );

	virtual void Create();
	virtual void Destroy();

	void AddChild( UIObject* object );
	void RemoveChild( UIObject* object );
	
	void SetWidth( const coord_t px );
	void SetHeight( const coord_t px );
	
	void SetScroll( vertex_t px );
	void SetScrollX( const coord_t px );
	void SetScrollY( const coord_t px );
	
	void SetInternalWidth( const coord_t px );
	void SetInternalHeight( const coord_t px );

private:
	
	coord_t m_scroll_speed = 10.0f;
	
	vertex_t m_internal_size = { 0, 0 };
	vertex_t m_scroll = { 0, 0 };
	struct {
		util::Timer timer;
		vertex_t scrolltarget = { 0, 0 };
	} m_smooth_scrolling;
	void UpdateInternalSize();
	
	std::vector< UIObject* > m_to_add = {};
	std::vector< UIObject* > m_to_remove = {};
	
	UIContainer* m_viewport = nullptr;
	Panel* m_body = nullptr;
	
	bool m_is_dragging = false;
	Vec2< ssize_t > m_drag_start_position = { 0, 0 };
	struct {
		const UIEventHandler* mouse_up;
		const UIEventHandler* mouse_move;
	} m_handlers;
};

}
}
