#include "Lifeforms.h"

#include "Clouds.h"

namespace game {
namespace mainmenu {

Lifeforms::Lifeforms( MainMenu *mainmenu ) : Menu( mainmenu, "NATIVE LIFE FORMS", {
	{ "RARE", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_LIFEFORMS_RARE;
		NextMenu( new Clouds( m_mainmenu ) );
	}},
	{ "AVERAGE", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_LIFEFORMS_AVERAGE;
		NextMenu( new Clouds( m_mainmenu ) );
	}},
	{ "ABUNDANT", [this] () -> void {
		m_mainmenu->m_settings.map_size = game::Settings::MAP_LIFEFORMS_ABUNDANT;
		NextMenu( new Clouds( m_mainmenu ) );
	}},
}) {}

}
}
