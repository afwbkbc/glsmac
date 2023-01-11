#include <cmath>

#include "Intro.h"

#include "game/mainmenu/MainMenu.h"

#include "engine/Engine.h"

using namespace ui::object;

namespace game {
namespace intro {

#define RANDNUM ( (float)rand() / (float)RAND_MAX )

void Intro::Start() {
	m_logo = new Image( "logo.pcx" );
	m_logo->SetAlign( UIObject::ALIGN_CENTER );
	g_engine->GetUI()->AddObject( m_logo );

	m_timer.SetTimeout(1000); // pretend we're loading something
	// TODO: do all preloading while intro is running?
}

void Intro::Stop() {
	g_engine->GetUI()->RemoveObject( m_logo );
}

void Intro::Iterate() {
	if (m_timer.Ticked()) {
		// switch to main menu
		g_engine->GetScheduler()->RemoveTask(this);
		g_engine->GetScheduler()->AddTask(new game::mainmenu::MainMenu);
	}
}

} /* namespace intro */
} /* namespace task */
