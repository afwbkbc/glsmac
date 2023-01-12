#pragma once

#include "object/UIContainer.h"

#include "util/Timer.h"
#include "types/Font.h"
#include "object/Label.h"

using namespace types;

namespace ui {

using namespace object;

CHILD_CLASS(UIDebugOverlay, UIContainer)

	void Create();
	void Destroy();
	void Iterate();

private:
	util::Timer m_fps_timer;
	size_t m_fps_frames = 0;
	Font *m_fps_counter_font = nullptr;
	Label *m_fps_counter = nullptr;

};

}
