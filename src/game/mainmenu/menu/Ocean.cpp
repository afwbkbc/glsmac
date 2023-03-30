#include "Ocean.h"

#include "Erosive.h"

namespace game {
namespace mainmenu {

Ocean::Ocean( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "SELECT OCEAN COVERAGE", {
	{ "30-50% OF SURFACE", [this] () -> void {
		m_mainmenu->m_settings.global.map.ocean = game::MapSettings::MAP_OCEAN_LOW;
		NEWV( menu, Erosive, m_mainmenu );
		NextMenu( menu );
	}},
	{ "50-70% OF SURFACE", [this] () -> void {
		m_mainmenu->m_settings.global.map.ocean = game::MapSettings::MAP_OCEAN_MEDIUM;
		NEWV( menu, Erosive, m_mainmenu );
		NextMenu( menu );
	}},
	{ "70-90% OF SURFACE", [this] () -> void {
		m_mainmenu->m_settings.global.map.ocean = game::MapSettings::MAP_OCEAN_HIGH;
		NEWV( menu, Erosive, m_mainmenu );
		NextMenu( menu );
	}},
}) {}

}
}
