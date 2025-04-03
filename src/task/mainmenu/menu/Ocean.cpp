#include "Ocean.h"

#include "Erosive.h"

#include "task/mainmenu/MainMenu.h"
#include "game/backend/State.h"
#include "game/backend/settings/Settings.h"

namespace task {
namespace mainmenu {

Ocean::Ocean( MainMenu* mainmenu )
	: CustomizeMapMenu(
	mainmenu, "SELECT OCEAN COVERAGE", 1, {
		{
			"30-50% OF SURFACE", {
									 CH( this ) {
										 m_mainmenu->m_state->m_settings.global.map.ocean_coverage = 0.4f;
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
										 m_mainmenu->m_state->m_settings.global.map.ocean_coverage = 0.6f;
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
										 m_mainmenu->m_state->m_settings.global.map.ocean_coverage = 0.8f;
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
