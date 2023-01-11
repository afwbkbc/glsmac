#include "Main.h"

#include "StartGame.h"
#include "Scenario.h"

#include "engine/Engine.h"

namespace task {
namespace mainmenu {

Main::Main( MainMenuTask *task ) : Menu( task, "", {
	{ "START GAME", [this] () -> void {
		m_task->m_settings.game_mode = game::Settings::GM_SINGLEPLAYER;
		NextMenu( new StartGame( m_task ) );
	}},
	{ "QUICK START", [this] () -> void {
		m_task->m_settings.game_mode = game::Settings::GM_SINGLEPLAYER;
		
		// randomize settings
		m_task->m_settings.map_type = game::Settings::MT_RANDOM;
		m_task->m_settings.map_size = rand() % 5 + 1;
		m_task->m_settings.map_ocean = rand() % 3 + 1;
		m_task->m_settings.map_erosive = rand() % 3 + 1;
		m_task->m_settings.map_lifeforms = rand() % 3 + 1;
		m_task->m_settings.map_clouds = rand() % 3 + 1;
		m_task->m_settings.difficulty = rand() % 6 + 1; // TODO: previous difficulty?
		m_task->m_settings.game_rules = game::Settings::GR_STANDARD;

		// TODO
	}},
	{ "SCENARIO", [this] () -> void {
		m_task->m_settings.game_mode = game::Settings::GM_SCENARIO;
		NextMenu( new Scenario( m_task ) );
	}},
	{ "LOAD GAME", [this] () -> void {
		// TODO
	}},
	{ "MULTIPLAYER", [this] () -> void {
		m_task->m_settings.game_mode = game::Settings::GM_MULTIPLAYER;
		// TODO
	}},
	{ "VIEW CREDITS", [this] () -> void {
		// TODO
	}},
	{ "EXIT GAME", [this] () -> void {
		g_engine->ShutDown();
	}},
}) {}

};

}
