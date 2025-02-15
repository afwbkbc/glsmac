#include "Common.h"

#include "engine/Engine.h"
#include "config/Config.h"
#include "ui_legacy/UI.h"
#include "ui_legacy/FPSCounter.h"
#include "ui_legacy/style/Theme.h"

namespace task {

void Common::Start() {
	auto* ui = g_engine->GetUI();

	NEW( m_theme, task::style::Theme );
	ui->AddTheme( m_theme );

	if ( g_engine->GetConfig()->HasLaunchFlag( config::Config::LF_SHOWFPS ) ) {
		NEW( m_fps_counter, ui_legacy::FPSCounter );

		ui->AddObject( m_fps_counter );
	}
}

void Common::Stop() {
	auto* ui = g_engine->GetUI();

	if ( m_fps_counter ) {
		ui->RemoveObject( m_fps_counter );
		m_fps_counter = nullptr;
	}

	ui->RemoveTheme( m_theme );
	DELETE( m_theme );
}

void Common::Iterate() {
}

}
