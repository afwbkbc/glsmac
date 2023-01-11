#include "Erosive.h"

#include "Lifeforms.h"

namespace task {
namespace mainmenu {

Erosive::Erosive( MainMenuTask *task ) : Menu( task, "ADJUST EROSIVE FORCES", {
	{ "STRONG", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_EROSIVE_STRONG;
		NextMenu( new Lifeforms( m_task ) );
	}},
	{ "AVERAGE", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_EROSIVE_AVERAGE;
		NextMenu( new Lifeforms( m_task ) );
	}},
	{ "WEAK", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_EROSIVE_WEAK;
		NextMenu( new Lifeforms( m_task ) );
	}},
}) {}

}
}
