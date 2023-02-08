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
		m_mainmenu->m_settings.global.map_ocean = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		m_mainmenu->m_settings.global.map_erosive = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		m_mainmenu->m_settings.global.map_lifeforms = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		m_mainmenu->m_settings.global.map_clouds = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		NEW( menu, Difficulty, m_mainmenu );
	}
	NextMenu( menu );
}

}
}
