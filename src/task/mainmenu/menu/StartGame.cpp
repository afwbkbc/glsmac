#include "StartGame.h"

#include "PlanetSize.h"

using namespace game;

namespace task {
namespace mainmenu {

StartGame::StartGame( MainMenuTask *task ) : Menu( task, "", {
	{ "MAKE RANDOM MAP", [this] () -> void {
		m_task->m_settings.map_type = Settings::MT_RANDOM;
		NextMenu( new PlanetSize( m_task ) );
	}},
	{ "CUSTOMIZE RANDOM MAP", [this] () -> void {
		m_task->m_settings.map_type = Settings::MT_CUSTOM;
		NextMenu( new PlanetSize( m_task ) );
	}},
	{ "THE MAP OF PLANET", [this] () -> void {
		m_task->m_settings.map_type = Settings::MT_PREGEN;
		// TODO
	}},
	{ "HUGE MAP OF PLANET", [this] () -> void {
		m_task->m_settings.map_type = Settings::MT_PREGEN;
		// TODO
	}},
	{ "LOAD MAP FILE", [this] () -> void {
		// TODO
	}}
}) {}

}
}
