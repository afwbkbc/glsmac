#include "PlanetSize.h"

#include "Ocean.h"
#include "Erosive.h"
#include "Lifeforms.h"
#include "Clouds.h"

#include "Difficulty.h"

namespace task {
namespace mainmenu {

PlanetSize::PlanetSize( MainMenuTask *task ) : Menu( task, "SELECT SIZE OF PLANET", {
	{ "TINY PLANET", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_TINY;
		ChooseNext();
	}},
	{ "SMALL PLANET", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_SMALL;
		ChooseNext();
	}},
	{ "STANDARD PLANET", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_STANDARD;
		ChooseNext();
	}},
	{ "LARGE PLANET", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_LARGE;
		ChooseNext();
	}},
	{ "HUGE PLANET", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_HUGE;
		ChooseNext();
	}},
	{ "HUGE PLANET", [this] () -> void {
		// TODO
	}}
}) {}

void PlanetSize::ChooseNext() {
	if (m_task->m_settings.map_type == game::Settings::MT_CUSTOM) {
		NextMenu( new Ocean( m_task ) );
	}
	else {
		// randomize settings
		m_task->m_settings.map_ocean = rand() % 3 + 1;
		m_task->m_settings.map_erosive = rand() % 3 + 1;
		m_task->m_settings.map_lifeforms = rand() % 3 + 1;
		m_task->m_settings.map_clouds = rand() % 3 + 1;
		NextMenu( new Difficulty( m_task ) );
	}
}

}
}
