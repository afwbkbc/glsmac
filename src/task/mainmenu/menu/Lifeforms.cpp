#include "Lifeforms.h"

#include "Clouds.h"

namespace task {
namespace mainmenu {

Lifeforms::Lifeforms( MainMenuTask *task ) : Menu( task, "NATIVE LIFE FORMS", {
	{ "RARE", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_LIFEFORMS_RARE;
		NextMenu( new Clouds( m_task ) );
	}},
	{ "AVERAGE", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_LIFEFORMS_AVERAGE;
		NextMenu( new Clouds( m_task ) );
	}},
	{ "ABUNDANT", [this] () -> void {
		m_task->m_settings.map_size = game::Settings::MAP_LIFEFORMS_ABUNDANT;
		NextMenu( new Clouds( m_task ) );
	}},
}) {}

}
}
