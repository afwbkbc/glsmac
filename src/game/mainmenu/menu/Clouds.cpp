#include "Clouds.h"

#include "Difficulty.h"

namespace game {
namespace mainmenu {

Clouds::Clouds( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "SELECT CLOUD COVER", {
	{ "SPARSE", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_CLOUDS_SPARSE;
		NEWV( menu, Difficulty, m_mainmenu );
		NextMenu( menu );
	}},
	{ "AVERAGE", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_CLOUDS_AVERAGE;
		NEWV( menu, Difficulty, m_mainmenu );
		NextMenu( menu );
	}},
	{ "DENSE", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_CLOUDS_DENSE;
		NEWV( menu, Difficulty, m_mainmenu );
		NextMenu( menu );
	}},
}) {}

}
}
