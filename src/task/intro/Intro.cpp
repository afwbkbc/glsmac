#include "Intro.h"

#include "task/mainmenu/MainMenu.h"

#include "engine/Engine.h"
#include "ui_legacy/UI.h"
#include "Theme.h"
#include "ui_legacy/object/Surface.h"
#include "scheduler/Scheduler.h"
#include "resource/ResourceManager.h"

namespace task {
namespace intro {

void Intro::Start() {

	if ( g_engine->GetResourceManager()->GetDetectedSMACType() == config::ST_LEGACY ) {
		// legacy doesn't have firaxis logo
		Finish();
	}
	else {
		NEW( m_theme, Theme );
		g_engine->GetUI()->AddTheme( m_theme );

		NEW( m_logo, ui_legacy::object::Surface, "IntroLogo" );
		g_engine->GetUI()->AddObject( m_logo );

		m_timer.SetTimeout( 1000 ); // pretend we're loading something
		// TODO: do all preloading while intro is running?
	}
}

void Intro::Stop() {
	if ( m_logo ) {
		g_engine->GetUI()->RemoveObject( m_logo );
		m_logo = nullptr;
	}

	if ( m_theme ) {
		g_engine->GetUI()->RemoveTheme( m_theme );
		m_theme = nullptr;
	}
	DELETE( m_theme );
}

void Intro::Iterate() {
	if ( m_timer.HasTicked() ) {
		Finish();
	}
}

void Intro::Finish() {
	// switch to main menu
	g_engine->GetScheduler()->RemoveTask( this );
	NEWV( task, task::mainmenu::MainMenu );
	g_engine->GetScheduler()->AddTask( task );
}

}
}
