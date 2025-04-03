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
							   m_mainmenu->m_state->m_settings.global.map.cloud_cover = 0.25f;
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
							   m_mainmenu->m_state->m_settings.global.map.cloud_cover = 0.5f;
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
							   m_mainmenu->m_state->m_settings.global.map.cloud_cover = 0.75f;
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
