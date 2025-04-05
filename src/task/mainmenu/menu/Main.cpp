#include "Main.h"

#include "StartGame.h"
#include "Scenario.h"
#include "Multiplayer.h"
#include "task/mainmenu/MainMenu.h"
#include "engine/Engine.h"
#include "game/backend/State.h"
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

									m_mainmenu->m_state->m_settings.local.game_mode = game::backend::settings::LocalSettings::GM_SINGLEPLAYER;
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

									m_mainmenu->m_state->m_settings.local.game_mode = game::backend::settings::LocalSettings::GM_SINGLEPLAYER;

									// randomize settings
									m_mainmenu->m_state->m_settings.global.Initialize();
									m_mainmenu->m_state->m_settings.global.map.type = game::backend::settings::MapSettings::MT_RANDOM;
									m_mainmenu->m_state->m_settings.global.map.size_x = 112;
									m_mainmenu->m_state->m_settings.global.map.size_y = 56;
									m_mainmenu->m_state->m_settings.global.map.ocean_coverage = m_mainmenu->GetRandom()->GetFloat( 0.2f, 0.8f );
									m_mainmenu->m_state->m_settings.global.map.erosive_forces = m_mainmenu->GetRandom()->GetFloat( 0.2f, 0.8f );
									m_mainmenu->m_state->m_settings.global.map.native_lifeforms = m_mainmenu->GetRandom()->GetFloat( 0.2f, 0.8f );
									m_mainmenu->m_state->m_settings.global.map.cloud_cover = m_mainmenu->GetRandom()->GetFloat( 0.2f, 0.8f );
									// TODO: previous rules and difficulty
									m_mainmenu->m_state->m_settings.global.difficulty_level = m_mainmenu->m_state->m_settings.global.rules.GetDefaultDifficultyLevelV();

									// start game
									m_mainmenu->InitSinglePlayer();
									m_mainmenu->StartGame();
								}
							}
		},
		{
			"SCENARIO",     {
								CH( this ) {
									m_mainmenu->m_state->m_settings.local.game_mode = game::backend::settings::LocalSettings::GM_SCENARIO;
									NEWV( menu, Scenario, m_mainmenu );
									NextMenu( menu );
								}
							}
		},
		{   "LOAD GAME",    {} },
		{
			"MULTIPLAYER",  {
								CH( this ) {
									m_mainmenu->m_state->m_settings.local.game_mode = game::backend::settings::LocalSettings::GM_MULTIPLAYER;
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
