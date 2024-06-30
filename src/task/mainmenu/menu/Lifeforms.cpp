#include "Lifeforms.h"

#include "Clouds.h"

#include "task/mainmenu/MainMenu.h"
#include "game/State.h"
#include "game/settings/Settings.h"

namespace task {
namespace mainmenu {

Lifeforms::Lifeforms( MainMenu* mainmenu )
	: CustomizeMapMenu(
	mainmenu, "NATIVE LIFE FORMS", 3, {
		{
			"RARE",     {
							CH( this ) {
								m_mainmenu->m_state->m_settings.global.map.lifeforms = game::settings::MapSettings::MAP_CONFIG_LIFEFORMS_RARE;
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
								m_mainmenu->m_state->m_settings.global.map.lifeforms = game::settings::MapSettings::MAP_CONFIG_LIFEFORMS_AVERAGE;
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
								m_mainmenu->m_state->m_settings.global.map.lifeforms = game::settings::MapSettings::MAP_CONFIG_LIFEFORMS_ABUNDANT;
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
