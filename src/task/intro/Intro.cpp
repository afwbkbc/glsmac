#include <cmath>

#include "Intro.h"

#include "task/mainmenu/MainMenu.h"

#include "engine/Engine.h"

using namespace ui::object;

namespace task {
namespace intro {

void Intro::Start() {

	g_engine->GetUI()->AddTheme( &m_theme );

	NEW( m_logo, Surface, "IntroLogo" );
	g_engine->GetUI()->AddObject( m_logo );

	m_timer.SetTimeout( 1000 ); // pretend we're loading something
	// TODO: do all preloading while intro is running?
}

void Intro::Stop() {
	g_engine->GetUI()->RemoveObject( m_logo );

	g_engine->GetUI()->RemoveTheme( &m_theme );
}

void Intro::Iterate() {
	if ( m_timer.HasTicked() ) {
		// switch to main menu
		g_engine->GetScheduler()->RemoveTask( this );
		NEWV( task, task::mainmenu::MainMenu );
		g_engine->GetScheduler()->AddTask( task );
	}
}

} /* namespace intro */
} /* namespace task */
