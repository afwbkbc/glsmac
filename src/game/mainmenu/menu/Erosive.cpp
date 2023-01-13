#include "Erosive.h"

#include "Lifeforms.h"

namespace game {
namespace mainmenu {

Erosive::Erosive( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "ADJUST EROSIVE FORCES", {
	{ "STRONG", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_EROSIVE_STRONG;
		NextMenu( new Lifeforms( m_mainmenu ) );
	}},
	{ "AVERAGE", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_EROSIVE_AVERAGE;
		NextMenu( new Lifeforms( m_mainmenu ) );
	}},
	{ "WEAK", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_EROSIVE_WEAK;
		NextMenu( new Lifeforms( m_mainmenu ) );
	}},
}) {}

}
}
