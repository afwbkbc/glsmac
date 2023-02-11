#pragma once

#include <vector>
#include <string>

#include "base/Task.h"

#include "util/Timer.h"
#include "types/Font.h"
#include "ui/object/Label.h"
#include "ui/object/Surface.h"

using namespace types;
using namespace ui;
namespace ui {
using namespace object;
}
namespace debug {

CLASS( DebugOverlay, base::Task )
	
	void Start();
	void Stop();
	void Iterate();

	void Show();
	void Hide();
	void Toggle();
	
	void Refresh();
	void ClearStats();
	
protected:
	util::Timer m_stats_timer;
	
	Texture* m_background_texture = nullptr;
	Surface* m_background_left = nullptr;
	Surface* m_background_middle = nullptr;
	
	size_t m_memory_stats_lines = 0;
	size_t m_font_size = 0;
	Font* m_stats_font = nullptr;
#define D( _stat ) Label* m_##_stats_label_##_stat = nullptr;
	DEBUG_STATS;
#undef D
	
	vector<Label*> m_memory_stats_labels = {};

	void ActivateLabel( Label* label, const size_t left, const size_t top );

private:
	bool m_is_visible = false;
	
	const UIEventHandler* m_toggle_handler = nullptr;
	
};

}
