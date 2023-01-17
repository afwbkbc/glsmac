#include "MainMenu.h"

#include <cmath>
#include <algorithm>

#include "engine/Engine.h"

#include "scene/actor/TextActor.h"

#include "menu/Main.h"
//#include "menu/Multiplayer.h"

using namespace std;
using namespace ui::object;
using namespace loader::texture;

namespace game {
namespace mainmenu {

void MainMenu::Start() {

	g_engine->GetUI()->SetTheme( &m_theme );
	
	// background
	NEW( m_background, Surface, "MainMenuBackground" );
	g_engine->GetUI()->AddObject( m_background );

	NEWV( menu, Main, this );
	//NEWV( menu, Multiplayer, this );
	ShowMenu( menu );
}

void MainMenu::GoBack() {
	if ( !m_menu_history.empty() ) {
		m_menu_object->Hide();
		DELETE( m_menu_object );
		m_menu_object = m_menu_history.back();
		m_menu_history.pop_back();
		m_menu_object->Show();
	}
	else {
		g_engine->ShutDown();
	}
}

void MainMenu::Stop() {
	
	if ( m_menu_object ) {
		m_menu_object->Hide();
		DELETE( m_menu_object );
		
		for (auto& it : m_menu_history) {
			DELETE( it );
		}
	}
	
	g_engine->GetUI()->RemoveObject( m_background );
	
	g_engine->GetUI()->UnsetTheme();

}

void MainMenu::ShowMenu( MenuObject* menu_object ) {
	if ( m_menu_object ) {
		m_menu_object->Hide();
		m_menu_history.push_back( m_menu_object );
	}
	m_menu_object = menu_object;
	m_menu_object->Show();
}


} /* namespace mainmenu */
} /* namespace game */
