#include "StartGame.h"

#include "PlanetSize.h"

using namespace game;

namespace game {
namespace mainmenu {

StartGame::StartGame( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "", {
	{ "MAKE RANDOM MAP", [this] () -> void {
		m_mainmenu->m_settings.map_type = Settings::MT_RANDOM;
		NextMenu( new PlanetSize( m_mainmenu ) );
	}},
	{ "CUSTOMIZE RANDOM MAP", [this] () -> void {
		m_mainmenu->m_settings.map_type = Settings::MT_CUSTOM;
		NextMenu( new PlanetSize( m_mainmenu ) );
	}},
	{ "THE MAP OF PLANET", [this] () -> void {
		m_mainmenu->m_settings.map_type = Settings::MT_PREGEN;
		// TODO
	}},
	{ "HUGE MAP OF PLANET", [this] () -> void {
		m_mainmenu->m_settings.map_type = Settings::MT_PREGEN;
		// TODO
	}},
	{ "LOAD MAP FILE", [this] () -> void {
		// TODO
	}}
}) {}

}
}
