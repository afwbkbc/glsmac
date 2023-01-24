#pragma once

#include "base/Task.h"

#include "ui/style/Theme.h"

namespace game {

CLASS( Common, base::Task )
	void Start();
	void Stop();
	void Iterate();
	
protected:
	game::style::Theme m_theme;

};

}
