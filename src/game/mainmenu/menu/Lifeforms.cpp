#include "Lifeforms.h"

#include "Clouds.h"

namespace game {
namespace mainmenu {

Lifeforms::Lifeforms( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "NATIVE LIFE FORMS", {
	{ "RARE", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_LIFEFORMS_RARE;
		NEWV( menu, Clouds, m_mainmenu );
		NextMenu( menu );
	}},
	{ "AVERAGE", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_LIFEFORMS_AVERAGE;
		NEWV( menu, Clouds, m_mainmenu );
		NextMenu( menu );
	}},
	{ "ABUNDANT", [this] () -> void {
		m_mainmenu->m_settings.global.map_size = game::GlobalSettings::MAP_LIFEFORMS_ABUNDANT;
		NEWV( menu, Clouds, m_mainmenu );
		NextMenu( menu );
	}},
}) {}

}
}
