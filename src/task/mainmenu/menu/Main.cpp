#include "Main.h"

#include "StartGame.h"
#include "Scenario.h"
#include "Multiplayer.h"
#include "task/mainmenu/MainMenu.h"
#include "engine/Engine.h"
#include "game/State.h"
#include "util/random/Random.h"

namespace task {
namespace mainmenu {

Main::Main( MainMenu* mainmenu )
	: SlidingMenu(
	mainmenu, "", {
		{
			"START GAME",   {
								CH( this ) {

									m_mainmenu->m_state->InitBindings();
									m_mainmenu->m_state->Configure();

									m_mainmenu->m_state->m_settings.local.game_mode = game::settings::LocalSettings::GM_SINGLEPLAYER;
									m_mainmenu->m_state->m_settings.global.Initialize();
									NEWV( menu, StartGame, m_mainmenu );
									NextMenu( menu );
								}
							}
		},
		{
			"QUICK START",  {
								CH( this ) {

									m_mainmenu->m_state->InitBindings();
									m_mainmenu->m_state->Configure();

									m_mainmenu->m_state->m_settings.local.game_mode = game::settings::LocalSettings::GM_SINGLEPLAYER;

									// randomize settings
									m_mainmenu->m_state->m_settings.global.Initialize();
									m_mainmenu->m_state->m_settings.global.map.type = game::settings::MapSettings::MT_RANDOM;
									m_mainmenu->m_state->m_settings.global.map.size = game::settings::MapSettings::MAP_CONFIG_STANDARD;
									m_mainmenu->m_state->m_settings.global.map.ocean = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
									m_mainmenu->m_state->m_settings.global.map.erosive = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
									m_mainmenu->m_state->m_settings.global.map.lifeforms = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
									m_mainmenu->m_state->m_settings.global.map.clouds = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
									// TODO: previous rules and difficulty
									m_mainmenu->m_state->m_settings.global.global_difficulty = m_mainmenu->m_state->m_settings.global.game_rules.GetDefaultDifficultyLevel();

									// start game
									m_mainmenu->InitSinglePlayer();
									m_mainmenu->StartGame();
								}
							}
		},
		{
			"SCENARIO",     {
								CH( this ) {
									m_mainmenu->m_state->m_settings.local.game_mode = game::settings::LocalSettings::GM_SCENARIO;
									NEWV( menu, Scenario, m_mainmenu );
									NextMenu( menu );
								}
							}
		},
		{   "LOAD GAME",    {} },
		{
			"MULTIPLAYER",  {
								CH( this ) {
									m_mainmenu->m_state->m_settings.local.game_mode = game::settings::LocalSettings::GM_MULTIPLAYER;
									NEWV( menu, Multiplayer, m_mainmenu );
									NextMenu( menu );
								}
							}
		},
		{   "VIEW CREDITS", {} },
		{
			"EXIT GAME",    {
								CH( this ) {
									g_engine->ShutDown();
								}
							}
		},
	}
) {}

};

}
