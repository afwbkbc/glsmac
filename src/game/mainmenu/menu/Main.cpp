#include "Main.h"

#include "StartGame.h"
#include "Scenario.h"

#include "Multiplayer.h"

#include "engine/Engine.h"

#include "game/world/World.h"

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
		m_mainmenu->m_settings.global.map_size = rand() % 5 + 1;
		m_mainmenu->m_settings.global.map_ocean = rand() % 3 + 1;
		m_mainmenu->m_settings.global.map_erosive = rand() % 3 + 1;
		m_mainmenu->m_settings.global.map_lifeforms = rand() % 3 + 1;
		m_mainmenu->m_settings.global.map_clouds = rand() % 3 + 1;
		m_mainmenu->m_settings.global.difficulty = rand() % 6 + 1; // TODO: previous difficulty?
		m_mainmenu->m_settings.global.game_rules = game::GlobalSettings::GR_STANDARD;

		// start game
		NEWV( task, game::world::World, m_mainmenu->m_settings );
		g_engine->GetScheduler()->RemoveTask( m_mainmenu );
		g_engine->GetScheduler()->AddTask( task );

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
