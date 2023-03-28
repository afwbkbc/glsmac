#include "StartGame.h"

#include "PlanetSize.h"
#include "LoadMapFile.h"

using namespace game;

namespace game {
namespace mainmenu {

StartGame::StartGame( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "", {
	{ "MAKE RANDOM MAP", [this] () -> void {
		m_mainmenu->m_settings.global.map_type = GlobalSettings::MT_RANDOM;
		NEWV( menu, PlanetSize, m_mainmenu );
		NextMenu( menu );
	}},
	{ "CUSTOMIZE RANDOM MAP", [this] () -> void {
		m_mainmenu->m_settings.global.map_type = GlobalSettings::MT_CUSTOM;
		NEWV( menu, PlanetSize, m_mainmenu );
		NextMenu( menu );
	}},
	{ "THE MAP OF PLANET", [this] () -> void {
		m_mainmenu->m_settings.global.map_type = GlobalSettings::MT_MAPFILE;
		MenuError();
	}},
	{ "HUGE MAP OF PLANET", [this] () -> void {
		m_mainmenu->m_settings.global.map_type = GlobalSettings::MT_MAPFILE;
		MenuError();
	}},
	{ "LOAD MAP FILE", [this] () -> void {
		m_mainmenu->m_settings.global.map_type = GlobalSettings::MT_MAPFILE;
		NEWV( menu, LoadMapFile, m_mainmenu );
		NextMenu( menu );
	}}
}) {}

}
}
