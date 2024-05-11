#pragma once

#include "base/Task.h"

namespace task {

namespace ui {
class FPSCounter;
}
namespace style {
class Theme;
}

CLASS( Common, base::Task )
	void Start() override;
	void Stop() override;
	void Iterate() override;

protected:
	style::Theme* m_theme;

	ui::FPSCounter* m_fps_counter = nullptr;

};

}
