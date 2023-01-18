#include "MainMenu.h"

#include <cmath>
#include <algorithm>

#include "engine/Engine.h"

#include "scene/actor/TextActor.h"

#include "menu/Main.h"
#include "menu/Multiplayer.h"

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

void MainMenu::Iterate() {
	if ( m_goback ) {
		m_goback = false;
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
	else if ( m_menu_next ) {
		if ( m_menu_object ) {
			m_menu_object->Hide();
			m_menu_history.push_back( m_menu_object );
		}
		m_menu_object = m_menu_next;
		m_menu_next = nullptr;
		m_menu_object->Show();
	}
}

void MainMenu::Stop() {
	
	if ( m_menu_object ) {
		m_menu_object->Hide();
		DELETE( m_menu_object );
		
	}
	
	for (auto& it : m_menu_history) {
		DELETE( it );
	}
	
	if ( m_menu_next ) {
		DELETE( m_menu_next );
	}
	
	g_engine->GetUI()->RemoveObject( m_background );
	
	g_engine->GetUI()->UnsetTheme();

}

void MainMenu::GoBack() {
#if DEBUG
	if ( m_goback ) {
		throw runtime_error( "goback already set" );
	}
#endif
	m_goback = true;
}

void MainMenu::ShowMenu( MenuObject* menu_object ) {
#if DEBUG
	if ( m_menu_next ) {
		throw runtime_error( "next menu already set" );
	}
#endif
	m_menu_next = menu_object;
}


} /* namespace mainmenu */
} /* namespace game */
