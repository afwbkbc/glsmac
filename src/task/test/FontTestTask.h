#pragma once

#include "TestTask.h"

#include "types/Font.h"
#include "scene/Scene.h"
#include "scene/actor/Actor.h"

namespace task {
namespace test {

MINOR_CLASS( Font, TestTask )
	void Start();
	void Stop();
	void Iterate();
protected:
	types::Font *m_font1;
	types::Font *m_font2;
	scene::actor::Actor *m_text1;
	scene::actor::Actor *m_text2;
	scene::actor::Actor *m_text3;
	scene::Scene *m_scene;
};

} /* namespace test */
} /* namespace task */
