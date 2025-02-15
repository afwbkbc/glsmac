#include "Console.h"

#include <mutex>

#include "engine/Engine.h"
#include "ui_legacy/UI.h"
#include "ui_legacy/theme/Theme.h"
#include "UI.h"
#include "Style.h"

namespace task {
namespace console {

static std::mutex s_console_mutex;
static Console* s_console = nullptr;
std::vector< std::string > s_console_prelog = {};

void Console::Start() {
#ifdef DEBUG
	{
		std::lock_guard< std::mutex > guard( s_console_mutex );
		ASSERT( !s_console, "console already set" );
	}
#endif

	auto* ui = g_engine->GetUI();

	NEW( m_theme, ::ui_legacy::theme::Theme );
	NEW( m_style, Style );
	m_theme->AddStyleSheet( m_style );
	ui->AddTheme( m_theme );

	NEW( m_ui, UI );
	ui->AddObject( m_ui );

	{
		std::lock_guard< std::mutex > guard( s_console_mutex );
		s_console = this;
		for ( const auto& text : s_console_prelog ) {
			s_console->m_ui->Log( text );
		}
		s_console_prelog.clear();
	}
}

void Console::Stop() {
	{
		std::lock_guard< std::mutex > guard( s_console_mutex );
		ASSERT( s_console, "console not set" );
		s_console = nullptr;
	}

	auto* ui = g_engine->GetUI();

	ui->RemoveObject( m_ui );

	ui->RemoveTheme( m_theme );
	DELETE( m_theme );
	DELETE( m_style );
}

void Console::Log( const std::string& text ) {
	std::lock_guard< std::mutex > guard( s_console_mutex );
	if ( s_console ) {
		s_console->m_ui->Log( text );
	}
	else {
		s_console_prelog.push_back( text );
	}
}

}
}
