#pragma once

#include <mutex>

#include "ui/object/UIContainer.h"

#include "util/Scroller.h"

namespace ui {
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

class UI : public ::ui::object::UIContainer {
public:
	UI();

	void Create() override;
	void Align() override;
	void Iterate() override;
	void Destroy() override;

	void Log( const std::string& text );

private:
	static constexpr size_t SLIDE_DURATION_MS = 100;
	static constexpr size_t HISTORY_LINES_LIMIT = 1000;
	static const std::string PROMPT_STR;

	bool m_is_active = false;
	bool m_is_visible = false;
	size_t m_last_height = 0;
	const ::ui::event::UIEventHandler* m_toggle_handler = nullptr;
	std::vector< const ::ui::event::UIEventHandler* > m_event_block_handlers = {};

	util::Scroller< coord_t > m_slide;

	::ui::object::Surface* m_background;
	::ui::object::Surface* m_border;
	::ui::object::Label* m_prompt;
	::ui::object::Input* m_input;
	::ui::object::TextView* m_history;

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
