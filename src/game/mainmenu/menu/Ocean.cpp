#include "Ocean.h"

#include "Erosive.h"

namespace game {
namespace mainmenu {

Ocean::Ocean( MainMenu *mainmenu ) : Menu( mainmenu, "SELECT OCEAN COVERAGE", {
	{ "30-50% OF SURFACE", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_OCEAN_SMALL;
		NextMenu( new Erosive( m_mainmenu ) );
	}},
	{ "50-70% OF SURFACE", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_OCEAN_AVERAGE;
		NextMenu( new Erosive( m_mainmenu ) );
	}},
	{ "70-90% OF SURFACE", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_OCEAN_LARGE;
		NextMenu( new Erosive( m_mainmenu ) );
	}},
}) {}

}
}
