#include "Clouds.h"

#include "Difficulty.h"

#include "task/mainmenu/MainMenu.h"
#include "game/State.h"
#include "game/settings/Settings.h"

namespace task {
namespace mainmenu {

Clouds::Clouds( MainMenu* mainmenu )
	: CustomizeMapMenu(
	mainmenu, "SELECT CLOUD COVER", 5, {
		{
			"SPARSE",  {
						   CH( this ) {
							   m_mainmenu->m_state->m_settings.global.map.clouds = game::settings::MapSettings::MAP_CONFIG_CLOUDS_SPARSE;
							   NEWV( menu, Difficulty, m_mainmenu );
							   NextMenu( menu );
						   },
						   CH( this ) {
							   SetPreviewVariant( '1' );
						   },
					   }
		},
		{
			"AVERAGE", {
						   CH( this ) {
							   m_mainmenu->m_state->m_settings.global.map.clouds = game::settings::MapSettings::MAP_CONFIG_CLOUDS_AVERAGE;
							   NEWV( menu, Difficulty, m_mainmenu );
							   NextMenu( menu );
						   },
						   CH( this ) {
							   SetPreviewVariant( '2' );
						   },
					   }
		},
		{
			"DENSE",   {
						   CH( this ) {
							   m_mainmenu->m_state->m_settings.global.map.clouds = game::settings::MapSettings::MAP_CONFIG_CLOUDS_DENSE;
							   NEWV( menu, Difficulty, m_mainmenu );
							   NextMenu( menu );
						   },
						   CH( this ) {
							   SetPreviewVariant( '3' );
						   },
					   }
		},
	}, 1
) {
	//
}

}
}
