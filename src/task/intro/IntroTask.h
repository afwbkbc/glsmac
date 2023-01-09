#pragma once

#include "../Task.h"

#include "types/Texture.h"
#include "ui/object/Image.h"

namespace task {

MINOR_CLASS( Intro, Task )
	void Start();
	void Stop();
	void Iterate();
	
protected:
	ui::object::Image *m_image;

};

} /* namespace task */
