#pragma once

#include <vector>

#include "Panel.h"

#include "types/Vec2.h"
#include "util/Scroller.h"

namespace ui {
namespace object {

class Scrollbar;
class SimpleButton;

CLASS( ScrollView, Panel )

	enum scrollview_type_t {
		ST_VERTICAL,
		ST_HORIZONTAL_INLINE,
	};

	ScrollView( const scrollview_type_t type, const std::string& class_name = "" );

	virtual void Create() override;
	virtual void Iterate() override;
	virtual void Destroy() override;

	void AddChild( UIObject* object );
	void RemoveChild( UIObject* object );

	void SetWidth( const coord_t px ) override;
	void SetHeight( const coord_t px ) override;

	const coord_t GetScrollX() const;
	const coord_t GetScrollY() const;

	void SetScroll( vertex_t px, const bool force = false );
	void SetScrollX( const coord_t px );
	void SetScrollY( const coord_t px );
	void ScrollToEnd();

	void ScrollToItem( UIObject* item );

	void SetScrollSpeed( const size_t scroll_speed );
	void SetSticky( const bool sticky );

	void SetInternalWidth( const coord_t px );
	void SetInternalHeight( const coord_t px );

protected:
	void ScrollToObjectMaybe( UIObject* object );

private:

	const scrollview_type_t m_type;

	size_t m_scroll_speed = 17;
	bool m_is_sticky = true;

	struct {
		Scrollbar* scrollbar = nullptr;
	} m_vertical;

	struct {
		SimpleButton* left_arrow = nullptr;
		SimpleButton* right_arrow = nullptr;
	} m_horizontal_inline;

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

	enum scroll_direction_t {
		SD_NONE,
		SD_LEFT,
		SD_RIGHT
	};
	scroll_direction_t m_scroll_direction = SD_NONE;

	UIContainer* m_viewport = nullptr;
	Panel* m_body = nullptr;
	std::unordered_set< UIObject* > m_items = {};

	bool m_need_body_refresh = false;

	bool m_is_dragging = false;
	types::Vec2< ssize_t > m_drag_start_position = {
		0,
		0
	};
	struct {
		const event::UIEventHandler* mouse_up;
		const event::UIEventHandler* mouse_move;
	} m_handlers;

	void AddChildToBody( UIObject* child );
	void RemoveChildFromBody( UIObject* child );
	void FixScrollY();

};

}
}
