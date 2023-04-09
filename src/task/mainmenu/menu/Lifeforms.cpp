#include "Lifeforms.h"

#include "Clouds.h"

namespace task {
namespace mainmenu {

Lifeforms::Lifeforms( MainMenu *mainmenu ) : CustomizeMapMenu( mainmenu, "NATIVE LIFE FORMS", 3, {
	{ "RARE", {
		CH( this ) {
			m_mainmenu->m_settings.global.map.lifeforms = game::MapSettings::MAP_LIFEFORMS_RARE;
			NEWV( menu, Clouds, m_mainmenu );
			NextMenu( menu );
		},
		CH( this ) {
			SetPreviewVariant( '1' );
		},
	}},
	{ "AVERAGE", {
		CH( this ) {
			m_mainmenu->m_settings.global.map.lifeforms = game::MapSettings::MAP_LIFEFORMS_AVERAGE;
			NEWV( menu, Clouds, m_mainmenu );
			NextMenu( menu );
		},
		CH( this ) {
			SetPreviewVariant( '2' );
		},
	}},
	{ "ABUNDANT", {
		CH( this ) {
			m_mainmenu->m_settings.global.map.lifeforms = game::MapSettings::MAP_LIFEFORMS_ABUNDANT;
			NEWV( menu, Clouds, m_mainmenu );
			NextMenu( menu );
		},
		CH( this ) {
			SetPreviewVariant( '3' );
		},
	}},
}, 1 ) {
	//
}

}
}
