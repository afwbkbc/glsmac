#pragma once

#include "TestTask.h"

#include "scene/Scene.h"
#include "scene/Camera.h"
#include "scene/actor/Actor.h"

namespace task {
namespace test {

MINOR_CLASS( World, TestTask )
	void Start();
	void Stop();
	void Iterate();
protected:
	scene::Scene *m_scene;
	scene::Camera *m_camera;
	scene::actor::Actor *m_planet;
};

} /* namespace test */
} /* namespace task */
