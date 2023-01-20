#include "MainMenu.h"

#include <cmath>
#include <algorithm>

#include "engine/Engine.h"

#include "menu/Main.h"
#include "menu/Error.h"

using namespace std;
using namespace ui::object;
using namespace loader::texture;

namespace game {
namespace mainmenu {

void MainMenu::Start() {

	auto* ui = g_engine->GetUI();
	
	ui->SetTheme( &m_theme );
	
	// background
	NEW( m_background, Surface, "MainMenuBackground" );
	ui->AddObject( m_background );

	m_mouse_handler = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_DOWN, EH( this ) {
		// rightclick = back
		if ( data->mouse.button == UIEvent::M_RIGHT && m_menu_object ) {
			if ( !m_menu_history.empty() ) { // don't exit game on right-click
				m_menu_object->MaybeClose(); // only sliding menus will close on right click
				return true;
			}
		}
		return false;
	});
	
	m_key_handler = ui->AddGlobalEventHandler( UIEvent::EV_KEY_DOWN, EH( this ) {
		// escape = back
		if ( data->key.code == UIEvent::K_ESCAPE && m_menu_object ) {
			m_menu_object->Close();
			return true;
		}
		return false;
	});

	NEW( m_music, SoundEffect, "MainMenuMusic" );
	g_engine->GetUI()->AddObject( m_music );
	
	//NEW( m_music_actor, scene::actor::SoundActor, "MainMenuMusic", g_engine->GetSoundLoader()->LoadSound( "opening menu.wav" ) );
		//m_music_actor->SetRepeatable( true );
	//g_engine->GetAudio()->AddActor( m_music_actor );
	
	NEWV( menu, Main, this );
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
			m_menu_object->SetChoice( m_choice_history.back() );
			m_choice_history.pop_back();
		}
		else {
			g_engine->ShutDown();
		}
	}
	else if ( m_menu_next ) {
		if ( m_menu_object ) {
			m_choice_history.push_back( m_menu_object->GetChoice() );
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
	
	g_engine->GetUI()->RemoveObject( m_music );
	
	auto* ui = g_engine->GetUI();
	
	ui->RemoveGlobalEventHandler( m_mouse_handler );
	ui->RemoveGlobalEventHandler( m_key_handler );
	
	ui->RemoveObject( m_background );
	
	ui->UnsetTheme();

}

void MainMenu::GoBack() {
	ASSERT( !m_goback, "goback already set" );
	m_goback = true;
}

void MainMenu::ShowMenu( MenuObject* menu_object ) {
	ASSERT( !m_menu_next, "next menu already set" );
	m_menu_next = menu_object;
}

void MainMenu::MenuError( const string& error_text ) {
	NEWV( menu, Error, this, error_text );
	ShowMenu( menu );
}



} /* namespace mainmenu */
} /* namespace game */
