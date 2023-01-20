#include "StartGame.h"

#include "PlanetSize.h"

using namespace game;

namespace game {
namespace mainmenu {

StartGame::StartGame( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "", {
	{ "MAKE RANDOM MAP", [this] () -> void {
		m_mainmenu->m_settings.map_type = Settings::MT_RANDOM;
		NEWV( menu, PlanetSize, m_mainmenu );
		NextMenu( menu );
	}},
	{ "CUSTOMIZE RANDOM MAP", [this] () -> void {
		m_mainmenu->m_settings.map_type = Settings::MT_CUSTOM;
		NEWV( menu, PlanetSize, m_mainmenu );
		NextMenu( menu );
	}},
	{ "THE MAP OF PLANET", [this] () -> void {
		m_mainmenu->m_settings.map_type = Settings::MT_PREGEN;
		MenuError( "This feature is not available yet." );
	}},
	{ "HUGE MAP OF PLANET", [this] () -> void {
		m_mainmenu->m_settings.map_type = Settings::MT_PREGEN;
		MenuError( "This feature is not available yet." );
	}},
	{ "LOAD MAP FILE", [this] () -> void {
		MenuError( "This feature is not available yet." );
	}}
}) {}

}
}
