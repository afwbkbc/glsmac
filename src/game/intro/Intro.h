#pragma once

#include "base/Task.h"

#include "types/Texture.h"
#include "ui/object/Image.h"
#include "util/Timer.h"

namespace game {
namespace intro {

CHILD_CLASS( Intro, base::Task )
	void Start();
	void Stop();
	void Iterate();
	
protected:
	ui::object::Image *m_logo;
	util::Timer m_timer;

};

} /* namespace intro */
} /* namespace game */
