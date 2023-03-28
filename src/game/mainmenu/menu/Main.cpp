#include "Main.h"

#include "StartGame.h"
#include "Scenario.h"

#include "Multiplayer.h"

#include "engine/Engine.h"

namespace game {
namespace mainmenu {

Main::Main( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "", {
	{ "START GAME", [this] () -> void {
		m_mainmenu->m_settings.global.game_mode = game::GlobalSettings::GM_SINGLEPLAYER;
		NEWV( menu, StartGame, m_mainmenu );
		NextMenu( menu );
	}},
	{ "QUICK START", [this] () -> void {
		m_mainmenu->m_settings.global.game_mode = game::GlobalSettings::GM_SINGLEPLAYER;
		
		// randomize settings
		m_mainmenu->m_settings.global.map_type = game::GlobalSettings::MT_RANDOM;
		m_mainmenu->m_settings.global.map_size = m_mainmenu->GetRandom()->GetUInt( 1, 5 );
		m_mainmenu->m_settings.global.map_ocean = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		m_mainmenu->m_settings.global.map_erosive = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		m_mainmenu->m_settings.global.map_lifeforms = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		m_mainmenu->m_settings.global.map_clouds = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		m_mainmenu->m_settings.global.difficulty = m_mainmenu->GetRandom()->GetUInt( 1, 6 ); // TODO: previous difficulty?
		m_mainmenu->m_settings.global.game_rules = game::GlobalSettings::GR_STANDARD;

		// start game
		m_mainmenu->StartGame();
	}},
	{ "SCENARIO", [this] () -> void {
		m_mainmenu->m_settings.global.game_mode = game::GlobalSettings::GM_SCENARIO;
		NEWV( menu, Scenario, m_mainmenu );
		NextMenu( menu );
	}},
	{ "LOAD GAME", [this] () -> void {
		MenuError();
	}},
	{ "MULTIPLAYER", [this] () -> void {
		m_mainmenu->m_settings.global.game_mode = game::GlobalSettings::GM_MULTIPLAYER;
		NEWV( menu, Multiplayer, m_mainmenu );
		NextMenu( menu );
	}},
	{ "VIEW CREDITS", [this] () -> void {
		MenuError();
	}},
	{ "EXIT GAME", [this] () -> void {
		g_engine->ShutDown();
	}},
}) {}

};

}
