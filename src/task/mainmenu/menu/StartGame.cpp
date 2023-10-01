#include "StartGame.h"

#include "PlanetSize.h"
#include "LoadMapFile.h"

using namespace task;

namespace task {
namespace mainmenu {

StartGame::StartGame( MainMenu* mainmenu )
	: SlidingMenu(
	mainmenu, "", {
		{
			"MAKE RANDOM MAP",      {
										CH( this ) {
											m_mainmenu->m_state->m_settings.global.map.type = game::MapSettings::MT_RANDOM;
											NEWV( menu, PlanetSize, m_mainmenu );
											NextMenu( menu );
										}
									}
		},
		{
			"CUSTOMIZE RANDOM MAP", {
										CH( this ) {
											m_mainmenu->m_state->m_settings.global.map.type = game::MapSettings::MT_CUSTOM;
											NEWV( menu, PlanetSize, m_mainmenu );
											NextMenu( menu );
										}
									}
		},
		{
			"THE MAP OF PLANET",    {
										CH( this ) {
											m_mainmenu->m_state->m_settings.global.map.type = game::MapSettings::MT_MAPFILE;
											MenuError();
										}
									}
		},
		{
			"HUGE MAP OF PLANET",   {
										CH( this ) {
											m_mainmenu->m_state->m_settings.global.map.type = game::MapSettings::MT_MAPFILE;
											MenuError();
										}
									}
		},
		{
			"LOAD MAP FILE",        {
										CH( this ) {
											m_mainmenu->m_state->m_settings.global.map.type = game::MapSettings::MT_MAPFILE;
											NEWV( menu, LoadMapFile, m_mainmenu );
											NextMenu( menu );
										}
									}
		}
	}
) {}

}
}
