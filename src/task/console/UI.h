#pragma once

#include <mutex>

#include "ui_legacy/object/UIContainer.h"

#include "util/Scroller.h"

namespace ui_legacy {
namespace object {
class Surface;
class Label;
class Input;
class TextView;
}
namespace event {
class UIEventHandler;
}
}

namespace task {
namespace console {

class UI : public ::ui_legacy::object::UIContainer {
public:
	UI();

	void Create() override;
	void Align() override;
	void Iterate() override;
	void Destroy() override;

	void Log( const std::string& text );

private:
	static constexpr size_t SLIDE_DURATION_MS = 100;
	static constexpr size_t HISTORY_LINES_LIMIT = 40; // TODO: fix FPS with greater values
	static const std::string PROMPT_STR;

	bool m_is_active = false;
	bool m_is_visible = false;
	size_t m_last_height = 0;
	const ::ui_legacy::event::UIEventHandler* m_toggle_handler = nullptr;
	std::vector< const ::ui_legacy::event::UIEventHandler* > m_event_block_handlers = {};

	util::Scroller< coord_t > m_slide;

	::ui_legacy::object::Surface* m_background;
	::ui_legacy::object::Surface* m_border;
	::ui_legacy::object::Label* m_prompt;
	::ui_legacy::object::Input* m_input;
	::ui_legacy::object::TextView* m_history;

	std::mutex m_history_mutex;
	std::vector< std::string > m_history_buffer;

	void ShowConsole();
	void HideConsole();
	void Toggle();

	const coord_t GetTopTarget() const;
	void ToggleAndSlide();
};

}
}
