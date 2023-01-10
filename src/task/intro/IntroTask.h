#pragma once

#include "../Task.h"

#include "types/Texture.h"
#include "ui/object/Image.h"
#include "util/Timer.h"

namespace task {
namespace intro {

MINOR_CLASS( Intro, Task )
	void Start();
	void Stop();
	void Iterate();
	
protected:
	ui::object::Image *m_logo;
	util::Timer m_timer;

};

} /* namespace intro */
} /* namespace task */
