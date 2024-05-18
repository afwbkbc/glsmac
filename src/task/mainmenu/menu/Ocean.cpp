#include "Ocean.h"

#include "Erosive.h"

#include "task/mainmenu/MainMenu.h"
#include "game/State.h"
#include "game/settings/Settings.h"

namespace task {
namespace mainmenu {

Ocean::Ocean( MainMenu* mainmenu )
	: CustomizeMapMenu(
	mainmenu, "SELECT OCEAN COVERAGE", 1, {
		{
			"30-50% OF SURFACE", {
									 CH( this ) {
										 m_mainmenu->m_state->m_settings.global.map.ocean = game::settings::MapSettings::MAP_CONFIG_OCEAN_LOW;
										 NEWV( menu, Erosive, m_mainmenu );
										 NextMenu( menu );
									 },
									 CH( this ) {
										 SetPreviewVariant( '1' );
									 },
								 }
		},
		{
			"50-70% OF SURFACE", {
									 CH( this ) {
										 m_mainmenu->m_state->m_settings.global.map.ocean = game::settings::MapSettings::MAP_CONFIG_OCEAN_MEDIUM;
										 NEWV( menu, Erosive, m_mainmenu );
										 NextMenu( menu );
									 },
									 CH( this ) {
										 SetPreviewVariant( '2' );
									 },
								 }
		},
		{
			"70-90% OF SURFACE", {
									 CH( this ) {
										 m_mainmenu->m_state->m_settings.global.map.ocean = game::settings::MapSettings::MAP_CONFIG_OCEAN_HIGH;
										 NEWV( menu, Erosive, m_mainmenu );
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
