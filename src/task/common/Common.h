#pragma once

#include "base/Task.h"

#include "ui/style/Theme.h"
#include "ui/FPSCounter.h"

namespace task {

CLASS( Common, base::Task )
	void Start() override;
	void Stop() override;
	void Iterate() override;

protected:
	task::style::Theme m_theme;

	ui::FPSCounter* m_fps_counter = nullptr;

};

}
