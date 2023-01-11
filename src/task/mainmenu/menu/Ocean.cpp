#include "Ocean.h"

#include "Erosive.h"

namespace task {
namespace mainmenu {

Ocean::Ocean( MainMenuTask *task ) : Menu( task, "SELECT OCEAN COVERAGE", {
	{ "30-50% OF SURFACE", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_OCEAN_SMALL;
		NextMenu( new Erosive( m_task ) );
	}},
	{ "50-70% OF SURFACE", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_OCEAN_AVERAGE;
		NextMenu( new Erosive( m_task ) );
	}},
	{ "70-90% OF SURFACE", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_OCEAN_LARGE;
		NextMenu( new Erosive( m_task ) );
	}},
}) {}

}
}
