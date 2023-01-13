#include "Clouds.h"

#include "Difficulty.h"

namespace game {
namespace mainmenu {

Clouds::Clouds( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "SELECT CLOUD COVER", {
	{ "SPARSE", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_CLOUDS_SPARSE;
		NextMenu( new Difficulty( m_mainmenu ) );
	}},
	{ "AVERAGE", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_CLOUDS_AVERAGE;
		NextMenu( new Difficulty( m_mainmenu ) );
	}},
	{ "DENSE", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_CLOUDS_DENSE;
		NextMenu( new Difficulty( m_mainmenu ) );
	}},
}) {}

}
}
