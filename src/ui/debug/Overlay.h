#pragma once

#include "ui/object/UIContainer.h"

#include <vector>
#include <string>

#include "util/Timer.h"
#include "types/Font.h"
#include "ui/object/Label.h"

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
	
	Font* m_stats_font = nullptr;
#define D( _stat ) Label* m_##_stats_label_##_stat = nullptr;
	DEBUG_STATS;
#undef D

};

}
}
