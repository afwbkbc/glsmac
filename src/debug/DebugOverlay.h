#pragma once

#include <vector>
#include <string>

#include "common/Task.h"

#include "util/Timer.h"

namespace types {
class Font;
namespace texture {
class Texture;
}
}

namespace ui {
namespace object {
class Label;
class Surface;
}
namespace event {
class UIEventHandler;
}
}

namespace debug {

CLASS( DebugOverlay, common::Task )

	void Start() override;
	void Stop() override;
	void Iterate() override;

	void Show();
	void Hide();
	void Toggle();

	void Refresh();
	void ClearStats();

protected:
	util::Timer m_stats_timer;

	types::texture::Texture* m_background_texture = nullptr;
	ui::object::Surface* m_background_left = nullptr;
	ui::object::Surface* m_background_middle = nullptr;

	size_t m_memory_stats_lines = 0;
	size_t m_font_size = 0;
	types::Font* m_stats_font = nullptr;

#define D( _stat ) ui::object::Label* m_##_stats_label_##_stat = nullptr;
	DEBUG_STATS;
#undef D

	std::vector< ui::object::Label* > m_memory_stats_labels = {};
	void ActivateLabel( ui::object::Label* label, const size_t left, const size_t top );

private:
	bool m_is_visible = false;

	const ui::event::UIEventHandler* m_toggle_handler = nullptr;

};

}
