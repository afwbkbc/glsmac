#include "PlanetSize.h"

#include "Ocean.h"
#include "Erosive.h"
#include "Lifeforms.h"
#include "Clouds.h"

#include "Difficulty.h"

namespace game {
namespace mainmenu {

PlanetSize::PlanetSize( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "SELECT SIZE OF PLANET", {
	{ "TINY PLANET", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_TINY;
		ChooseNext();
	}},
	{ "SMALL PLANET", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_SMALL;
		ChooseNext();
	}},
	{ "STANDARD PLANET", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_STANDARD;
		ChooseNext();
	}},
	{ "LARGE PLANET", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_LARGE;
		ChooseNext();
	}},
	{ "HUGE PLANET", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_HUGE;
		ChooseNext();
	}},
	{ "CUSTOM SIZE", [this] () -> void {
		MenuError();
	}}
}) {}

void PlanetSize::ChooseNext() {
	SlidingMenu *menu;
	if (m_mainmenu->m_settings.global.map_type == game::GlobalSettings::MT_CUSTOM) {
		NEW( menu, Ocean, m_mainmenu );
	}
	else {
		// randomize settings
		m_mainmenu->m_settings.global.map_ocean = rand() % 3 + 1;
		m_mainmenu->m_settings.global.map_erosive = rand() % 3 + 1;
		m_mainmenu->m_settings.global.map_lifeforms = rand() % 3 + 1;
		m_mainmenu->m_settings.global.map_clouds = rand() % 3 + 1;
		NEW( menu, Difficulty, m_mainmenu );
	}
	NextMenu( menu );
}

}
}
