#include <cmath>

#include "IntroTask.h"

#include "../mainmenu/MainMenuTask.h"

#include "engine/Engine.h"

using namespace ui::object;

namespace task {
namespace intro {

#define RANDNUM ( (float)rand() / (float)RAND_MAX )

void IntroTask::Start() {
	m_logo = new Image( "logo.pcx" );
	m_logo->SetAlign( UIObject::ALIGN_CENTER );
	g_engine->GetUI()->AddObject( m_logo );

	m_timer.SetTimeout(1); // pretend we're loading something
}

void IntroTask::Stop() {
	g_engine->GetUI()->RemoveObject( m_logo );
}

void IntroTask::Iterate() {
	if (m_timer.Ticked()) {
		// switch to main menu
		g_engine->GetScheduler()->RemoveTask(this);
		g_engine->GetScheduler()->AddTask(new mainmenu::MainMenuTask);
	}
}

} /* namespace intro */
} /* namespace task */
