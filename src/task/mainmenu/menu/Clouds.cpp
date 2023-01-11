#include "Clouds.h"

#include "Difficulty.h"

namespace task {
namespace mainmenu {

Clouds::Clouds( MainMenuTask *task ) : Menu( task, "SELECT CLOUD COVER", {
	{ "SPARSE", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_CLOUDS_SPARSE;
		NextMenu( new Difficulty( m_task ) );
	}},
	{ "AVERAGE", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_CLOUDS_AVERAGE;
		NextMenu( new Difficulty( m_task ) );
	}},
	{ "DENSE", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_CLOUDS_DENSE;
		NextMenu( new Difficulty( m_task ) );
	}},
}) {}

}
}
