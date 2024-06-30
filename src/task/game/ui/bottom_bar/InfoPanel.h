#pragma once

#include <vector>
#include <string>

#include "BBSection.h"

#include "util/Timer.h"

namespace ui::object {
class Label;
}

namespace task {
namespace game {
namespace ui {

CLASS( InfoPanel, BBSection )

	static constexpr size_t s_character_frequency_ms = 100;
	static constexpr size_t s_wait_after_print = 10000;

	// TODO: load from styles?
	static constexpr size_t s_label_height = 15;
	static constexpr size_t s_padding = 5;

	enum info_type_t {
		IT_NONE,
		IT_MISSION_STATS,

		IT_MAX
	};

	InfoPanel( Game* game );

	void Create() override;
	void Iterate() override;
	void Destroy() override;

private:
	info_type_t m_info_type = IT_NONE;

	void Restart();

	std::vector< std::string > m_lines = {};
	std::vector< ::ui::object::Label* > m_labels = {};
	size_t m_line_index = 0;
	size_t m_line_position = 0;
	util::Timer m_timer;
};

}
}
}
