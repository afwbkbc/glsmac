#include "PlanetSize.h"

#include "Ocean.h"

#include "Difficulty.h"
#include "task/mainmenu/MainMenu.h"
#include "game/backend/State.h"
#include "game/backend/settings/Settings.h"
#include "util/random/Random.h"

namespace task {
namespace mainmenu {

PlanetSize::PlanetSize( MainMenu* mainmenu )
	: SlidingMenu(
	mainmenu, "SELECT SIZE OF PLANET", {
		{
			"TINY PLANET",     {
								   CH( this ) {
									   m_mainmenu->m_state->m_settings.global.map.size_x = 68;
									   m_mainmenu->m_state->m_settings.global.map.size_y = 34;
									   ChooseNext();
								   }
							   }
		},
		{
			"SMALL PLANET",    {
								   CH( this ) {
									   m_mainmenu->m_state->m_settings.global.map.size_x = 88;
									   m_mainmenu->m_state->m_settings.global.map.size_y = 44;
									   ChooseNext();
								   }
							   }
		},
		{
			"STANDARD PLANET", {
								   CH( this ) {
									   m_mainmenu->m_state->m_settings.global.map.size_x = 112;
									   m_mainmenu->m_state->m_settings.global.map.size_y = 56;
									   ChooseNext();
								   }
							   }
		},
		{
			"LARGE PLANET",    {
								   CH( this ) {
									   m_mainmenu->m_state->m_settings.global.map.size_x = 140;
									   m_mainmenu->m_state->m_settings.global.map.size_y = 70;
									   ChooseNext();
								   }
							   }
		},
		{
			"HUGE PLANET",     {
								   CH( this ) {
									   m_mainmenu->m_state->m_settings.global.map.size_x = 180;
									   m_mainmenu->m_state->m_settings.global.map.size_y = 90;
									   ChooseNext();
								   }
							   }
		},
		{   "CUSTOM SIZE",     {} },
	}, 2
) {
	//
}

void PlanetSize::ChooseNext() {
	SlidingMenu* menu;
	if ( m_mainmenu->m_state->m_settings.global.map.type == game::backend::settings::MapSettings::MT_CUSTOM ) {
		NEW( menu, Ocean, m_mainmenu );
	}
	else {
		// randomize settings
		m_mainmenu->m_state->m_settings.global.map.ocean_coverage = m_mainmenu->GetRandom()->GetFloat( 0.2f, 0.8f );
		m_mainmenu->m_state->m_settings.global.map.erosive_forces = m_mainmenu->GetRandom()->GetFloat( 0.2f, 0.8f );
		m_mainmenu->m_state->m_settings.global.map.native_lifeforms = m_mainmenu->GetRandom()->GetFloat( 0.2f, 0.8f );
		m_mainmenu->m_state->m_settings.global.map.cloud_cover = m_mainmenu->GetRandom()->GetFloat( 0.2f, 0.8f );
		NEW( menu, Difficulty, m_mainmenu );
	}
	NextMenu( menu );
}

}
}
