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

protected:
	const bool ProcessEvent( GSE_CALLABLE, const input::Event& event ) override;

	void SetHasVScroll( GSE_CALLABLE, const bool value );
	void SetHasHScroll( GSE_CALLABLE, const bool value );

	// TODO: refactor
	std::function< void() > m_on_scrollview_resize = nullptr;
	Scrollbar* m_vscroll = nullptr;
	Scrollbar* m_hscroll = nullptr;

private:

	Container* m_inner = nullptr;

	bool m_has_vscroll = true;
	bool m_has_hscroll = true;

	void UpdateScrollbars( const size_t width, const size_t height );

	size_t m_padding = 0;
	void SetPadding( GSE_CALLABLE, const int padding );

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
