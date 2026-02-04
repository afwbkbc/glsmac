#pragma once

#include "Panel.h"

#include <functional>

namespace ui {
namespace dom {

class Scrollbar;
class Container;

class Scrollview : public Panel {
public:
	Scrollview( DOM_ARGS_TD( "scrollview" ), const bool factories_allowed = true );

	void Destroy( GSE_CALLABLE ) override;

protected:
	const bool ProcessEvent( GSE_CALLABLE, const input::Event& event ) override;

	void SetHasVScroll( GSE_CALLABLE, const bool value );
	void SetHasHScroll( GSE_CALLABLE, const bool value );

	// TODO: refactor
	std::function< void() > m_on_scrollview_resize = nullptr;
	Scrollbar* m_vscroll = nullptr;
	Scrollbar* m_hscroll = nullptr;

	size_t m_padding = 0;

	bool m_autoscroll = false;

private:
	
	Container* m_inner = nullptr;

	bool m_has_vscroll = true;
	bool m_has_hscroll = true;

	void UpdateScrollbars( size_t width = 0, size_t height = 0 );

	void SetPadding( GSE_CALLABLE, const int padding );
	void SetAutoScroll( GSE_CALLABLE, const bool value );

	bool m_is_updating = false;

	struct {
		std::function< const bool( GSE_CALLABLE, const input::Event& event ) > handler;
		bool is_dragging = false;
		int initial_offset_x = 0;
		int initial_offset_y = 0;
		size_t drag_handler_id = 0;
	} m_drag;

};

}
}
