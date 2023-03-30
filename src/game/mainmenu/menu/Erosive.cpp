#include "Erosive.h"

#include "Lifeforms.h"

namespace game {
namespace mainmenu {

Erosive::Erosive( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "ADJUST EROSIVE FORCES", {
	{ "STRONG", [this] () -> void {
		m_mainmenu->m_settings.global.map.erosive = game::MapSettings::MAP_EROSIVE_STRONG;
		NEWV( menu, Lifeforms, m_mainmenu );
		NextMenu( menu );
	}},
	{ "AVERAGE", [this] () -> void {
		m_mainmenu->m_settings.global.map.erosive = game::MapSettings::MAP_EROSIVE_AVERAGE;
		NEWV( menu, Lifeforms, m_mainmenu );
		NextMenu( menu );
	}},
	{ "WEAK", [this] () -> void {
		m_mainmenu->m_settings.global.map.erosive = game::MapSettings::MAP_EROSIVE_WEAK;
		NEWV( menu, Lifeforms, m_mainmenu );
		NextMenu( menu );
	}},
}) {}

}
}
