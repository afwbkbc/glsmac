#include "Ocean.h"

#include "Erosive.h"

namespace game {
namespace mainmenu {

Ocean::Ocean( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "SELECT OCEAN COVERAGE", {
	{ "30-50% OF SURFACE", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_OCEAN_SMALL;
		NEWV( menu, Erosive, m_mainmenu );
		NextMenu( menu );
	}},
	{ "50-70% OF SURFACE", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_OCEAN_AVERAGE;
		NEWV( menu, Erosive, m_mainmenu );
		NextMenu( menu );
	}},
	{ "70-90% OF SURFACE", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_OCEAN_LARGE;
		NEWV( menu, Erosive, m_mainmenu );
		NextMenu( menu );
	}},
}) {}

}
}
