#include "Common.h"

#include "engine/Engine.h"

using namespace ui::object;

namespace task {

void Common::Start() {
	auto* ui = g_engine->GetUI();

	ui->AddTheme( &m_theme );

	if ( g_engine->GetConfig()->HasLaunchFlag( config::Config::LF_SHOWFPS ) ) {
		NEW( m_fps_counter, ui::FPSCounter );

		ui->AddObject( m_fps_counter );
	}
}

void Common::Stop() {
	auto* ui = g_engine->GetUI();

	if ( m_fps_counter ) {
		ui->RemoveObject( m_fps_counter );
		m_fps_counter = nullptr;
	}

	ui->RemoveTheme( &m_theme );
}

void Common::Iterate() {
}

}
