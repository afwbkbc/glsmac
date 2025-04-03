#include "Lifeforms.h"

#include "Clouds.h"

#include "task/mainmenu/MainMenu.h"
#include "game/backend/State.h"
#include "game/backend/settings/Settings.h"

namespace task {
namespace mainmenu {

Lifeforms::Lifeforms( MainMenu* mainmenu )
	: CustomizeMapMenu(
	mainmenu, "NATIVE LIFE FORMS", 3, {
		{
			"RARE",     {
							CH( this ) {
								m_mainmenu->m_state->m_settings.global.map.native_lifeforms = 0.25f;
								NEWV( menu, Clouds, m_mainmenu );
								NextMenu( menu );
							},
							CH( this ) {
								SetPreviewVariant( '1' );
							},
						}
		},
		{
			"AVERAGE",  {
							CH( this ) {
								m_mainmenu->m_state->m_settings.global.map.native_lifeforms = 0.5f;
								NEWV( menu, Clouds, m_mainmenu );
								NextMenu( menu );
							},
							CH( this ) {
								SetPreviewVariant( '2' );
							},
						}
		},
		{
			"ABUNDANT", {
							CH( this ) {
								m_mainmenu->m_state->m_settings.global.map.native_lifeforms = 0.75f;
								NEWV( menu, Clouds, m_mainmenu );
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
