#include <cmath>
#include <cstdlib>

#include "ManagerTestTask.h"

#include "engine/Engine.h"

#include "task/test/RendererTestTask.h"
#include "task/test/FontTestTask.h"
#include "task/test/UITestTask.h"
#include "task/test/WorldTestTask.h"

namespace task {
namespace test {

#define RANDNUM ( (float)rand() / (float)RAND_MAX )

void ManagerTestTask::Start() {
/*	m_scene = new scene::Scene( "RendererTest", scene::SCENE_TYPE_ORTHO );
	m_logo = new scene::actor::MeshActor( "Logo", testmesh2() );
	m_scene->AddActor( m_logo );
	g_engine->GetRenderer()->AddScene( m_scene );*/
	
	m_timer.SetInterval(rand() % 5000 + 1);
}

void ManagerTestTask::Stop() {
/*	g_engine->GetRenderer()->RemoveScene( m_scene );
	delete m_logo;
	delete m_scene;*/
}

void ManagerTestTask::Iterate() {
	
	if (m_timer.Ticked()) {

		if (m_task) {
			g_engine->GetScheduler()->RemoveTask(m_task);
		}
		
		m_task_num = rand() % 4;
		
		switch (m_task_num) {
			case 0: {
				m_task = new task::test::FontTestTask;
				break;
			}
			case 1: {
				m_task = new task::test::RendererTestTask;
				break;
			}
			case 2: {
				m_task = new task::test::UITestTask;
				break;
			}
			case 3: {
				m_task = new task::test::WorldTestTask;
				break;
			}
		}
		g_engine->GetScheduler()->AddTask(m_task);
		
	}
	
}

} /* namespace test */
} /* namespace task */
