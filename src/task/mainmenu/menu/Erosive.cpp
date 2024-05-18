#include "Erosive.h"

#include "Lifeforms.h"

#include "task/mainmenu/MainMenu.h"
#include "game/State.h"
#include "game/settings/Settings.h"

namespace task {
namespace mainmenu {

Erosive::Erosive( MainMenu* mainmenu )
	: SlidingMenu(
	mainmenu, "ADJUST EROSIVE FORCES", {
		{
			"STRONG",  {
						   CH( this ) {
							   m_mainmenu->m_state->m_settings.global.map.erosive = game::settings::MapSettings::MAP_CONFIG_EROSIVE_STRONG;
							   NEWV( menu, Lifeforms, m_mainmenu );
							   NextMenu( menu );
						   },
						   CH( this ) {
							   SetMoonsVariant( '1' );
						   }
					   }
		},
		{
			"AVERAGE", {
						   CH( this ) {
							   m_mainmenu->m_state->m_settings.global.map.erosive = game::settings::MapSettings::MAP_CONFIG_EROSIVE_AVERAGE;
							   NEWV( menu, Lifeforms, m_mainmenu );
							   NextMenu( menu );
						   },
						   CH( this ) {
							   SetMoonsVariant( '2' );
						   }
					   }
		},
		{
			"WEAK",    {
						   CH( this ) {
							   m_mainmenu->m_state->m_settings.global.map.erosive = game::settings::MapSettings::MAP_CONFIG_EROSIVE_WEAK;
							   NEWV( menu, Lifeforms, m_mainmenu );
							   NextMenu( menu );
						   },
						   CH( this ) {
							   SetMoonsVariant( '3' );
						   }
					   }
		},
	}, 1
) {
	//
}

void Erosive::Close() {
	if ( IsReadyToClose() ) {
		m_mainmenu->SetCustomizeMapMoons( "" );
		SlidingMenu::Close();
	}
}

void Erosive::SetMoonsVariant( const char variant ) {
	m_mainmenu->SetCustomizeMapMoons( (std::string)"moon" + variant );
}

}
}
