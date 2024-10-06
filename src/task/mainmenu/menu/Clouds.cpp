#include "Clouds.h"

#include "Difficulty.h"

#include "task/mainmenu/MainMenu.h"
#include "game/backend/State.h"
#include "game/backend/settings/Settings.h"

namespace task {
namespace mainmenu {

Clouds::Clouds( MainMenu* mainmenu )
	: CustomizeMapMenu(
	mainmenu, "SELECT CLOUD COVER", 5, {
		{
			"SPARSE",  {
						   CH( this ) {
							   m_mainmenu->m_state->m_settings.global.map.clouds = game::backend::settings::MapSettings::MAP_CONFIG_CLOUDS_SPARSE;
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
							   m_mainmenu->m_state->m_settings.global.map.clouds = game::backend::settings::MapSettings::MAP_CONFIG_CLOUDS_AVERAGE;
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
							   m_mainmenu->m_state->m_settings.global.map.clouds = game::backend::settings::MapSettings::MAP_CONFIG_CLOUDS_DENSE;
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
