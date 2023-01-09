#pragma once

#include "Task.h"

#include "scene/Scene.h"

namespace task {

MINOR_CLASS( Intro, Task )
	void Start();
	void Stop();
	void Iterate();
protected:
	scene::Scene *m_scene;
	scene::actor::Actor *m_logo;
};

} /* namespace task */
