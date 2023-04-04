#pragma once

#include "base/Task.h"

#include "types/Texture.h"
#include "ui/object/Surface.h"
#include "util/Timer.h"

#include "Theme.h"

namespace task {
namespace intro {

CLASS( Intro, base::Task )
	void Start();
	void Stop();
	void Iterate();
	
protected:
	Theme m_theme;
	::ui::object::Surface *m_logo = nullptr;
	util::Timer m_timer;

};

} /* namespace intro */
} /* namespace game */
