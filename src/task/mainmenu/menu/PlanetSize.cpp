#include "PlanetSize.h"

#include "Ocean.h"

#include "Difficulty.h"

namespace task {
namespace mainmenu {

PlanetSize::PlanetSize( MainMenu* mainmenu )
	: SlidingMenu(
	mainmenu, "SELECT SIZE OF PLANET", {
		{
			"TINY PLANET",     {
								   CH( this ) {
									   m_mainmenu->m_state.m_settings.global.map.size = game::MapSettings::MAP_TINY;
									   ChooseNext();
								   }
							   }
		},
		{
			"SMALL PLANET",    {
								   CH( this ) {
									   m_mainmenu->m_state.m_settings.global.map.size = game::MapSettings::MAP_SMALL;
									   ChooseNext();
								   }
							   }
		},
		{
			"STANDARD PLANET", {
								   CH( this ) {
									   m_mainmenu->m_state.m_settings.global.map.size = game::MapSettings::MAP_STANDARD;
									   ChooseNext();
								   }
							   }
		},
		{
			"LARGE PLANET",    {
								   CH( this ) {
									   m_mainmenu->m_state.m_settings.global.map.size = game::MapSettings::MAP_LARGE;
									   ChooseNext();
								   }
							   }
		},
		{
			"HUGE PLANET",     {
								   CH( this ) {
									   m_mainmenu->m_state.m_settings.global.map.size = game::MapSettings::MAP_HUGE;
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
	if ( m_mainmenu->m_state.m_settings.global.map.type == game::MapSettings::MT_CUSTOM ) {
		NEW( menu, Ocean, m_mainmenu );
	}
	else {
		// randomize settings
		m_mainmenu->m_state.m_settings.global.map.ocean = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		m_mainmenu->m_state.m_settings.global.map.erosive = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		m_mainmenu->m_state.m_settings.global.map.lifeforms = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		m_mainmenu->m_state.m_settings.global.map.clouds = m_mainmenu->GetRandom()->GetUInt( 1, 3 );
		NEW( menu, Difficulty, m_mainmenu );
	}
	NextMenu( menu );
}

}
}
