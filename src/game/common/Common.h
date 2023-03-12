#pragma once

#include "base/Task.h"

#include "ui/style/Theme.h"
#include "ui/FPSCounter.h"

namespace game {

CLASS( Common, base::Task )
	void Start();
	void Stop();
	void Iterate();
	
protected:
	game::style::Theme m_theme;

	ui::FPSCounter *m_fps_counter = nullptr;
	
};

}
