#pragma once

#include "Task.h"

#include "types/Texture.h"
#include "ui/object/Panel.h"

namespace task {

MINOR_CLASS( Intro, Task )
	void Start();
	void Stop();
	void Iterate();
	
protected:
	ui::object::Panel *m_panel;
	types::Texture *m_logo_image;

};

} /* namespace task */
