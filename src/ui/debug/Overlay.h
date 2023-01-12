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
	util::Timer m_fps_timer;
	size_t m_fps_frames = 0;
	Font *m_fps_counter_font = nullptr;
	Label *m_fps_counter = nullptr;

};

}
}
