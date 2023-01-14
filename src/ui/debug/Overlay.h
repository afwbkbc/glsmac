#pragma once

#include "ui/object/UIContainer.h"

#include <vector>
#include <string>

#include "util/Timer.h"
#include "types/Font.h"
#include "ui/object/Label.h"
#include "ui/object/Surface.h"

using namespace types;

namespace ui {

using namespace object;

namespace debug {
	
CHILD_CLASS(Overlay, UIContainer)

	void Create();
	void Destroy();
	void Iterate();

private:
	util::Timer m_stats_timer;
	
	Texture* m_background_texture = nullptr;
	Surface* m_background = nullptr;
	
	size_t m_memory_stats_lines = 0;
	size_t m_font_size = 0;
	Font* m_stats_font = nullptr;
#define D( _stat ) Label* m_##_stats_label_##_stat = nullptr;
	DEBUG_STATS;
#undef D
	
	vector<Label*> m_memory_stats_labels = {};

	void ActivateLabel( Label* label, const size_t left, const size_t top );
};

}
}
