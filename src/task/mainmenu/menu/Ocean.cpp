#include "Ocean.h"

#include "Erosive.h"

namespace task {
namespace mainmenu {

Ocean::Ocean( MainMenu* mainmenu )
	: CustomizeMapMenu(
	mainmenu, "SELECT OCEAN COVERAGE", 1, {
		{
			"30-50% OF SURFACE", {
									 CH( this ) {
										 m_mainmenu->m_settings.global.map.ocean = game::MapSettings::MAP_OCEAN_LOW;
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
										 m_mainmenu->m_settings.global.map.ocean = game::MapSettings::MAP_OCEAN_MEDIUM;
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
										 m_mainmenu->m_settings.global.map.ocean = game::MapSettings::MAP_OCEAN_HIGH;
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
