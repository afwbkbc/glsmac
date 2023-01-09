#pragma once

#include "TestTask.h"

#include "scene/Scene.h"
#include "scene/actor/Actor.h"

namespace task {
namespace test {

MINOR_CLASS( Renderer, TestTask )
	void Start();
	void Stop();
	void Iterate();
protected:
	scene::Scene *m_scene;
	scene::actor::Actor *m_logo;
};

} /* namespace test */
} /* namespace task */
