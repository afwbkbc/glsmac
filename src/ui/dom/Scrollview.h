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

private:

	Container* m_inner = nullptr;
	Scrollbar* m_vscroll = nullptr;
	Scrollbar* m_hscroll = nullptr;

	void UpdateScrollbars( const size_t width, const size_t height );

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
