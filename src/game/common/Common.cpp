#include "Common.h"

#include "engine/Engine.h"

using namespace ui::object;

namespace game {

void Common::Start() {
	
	g_engine->GetUI()->AddTheme( &m_theme );
}

void Common::Stop() {
	g_engine->GetUI()->RemoveTheme( &m_theme );
}

void Common::Iterate() {
}

}
