#pragma once

#include "common/Task.h"

namespace task {

namespace ui_legacy {
class FPSCounter;
}
namespace style {
class Theme;
}

CLASS( Common, common::Task )
	void Start() override;
	void Stop() override;
	void Iterate() override;

protected:
	style::Theme* m_theme;

	ui_legacy::FPSCounter* m_fps_counter = nullptr;

};

}
