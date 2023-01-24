#include "Join.h"

#include "engine/Engine.h"

#include "types/ThreadCall.h"

#include "Lobby.h"

namespace game {
namespace mainmenu {

Join::Join( MainMenu* mainmenu ) : PopupMenu( mainmenu, "JOIN MULTIPLAYER GAME" ) {
	SetWidth( 500 );
	SetHeight( 148 );
	SetFlags( { PF_HAS_OK, PF_HAS_CANCEL } );
}

Join::~Join() {
}

void Join::Show() {
	PopupMenu::Show();
	
	NEW( m_section, Section, "PopupSection" );
		m_section->SetPadding( 4 );
	m_body->AddChild( m_section );
	
	NEW( m_label_yourname, Label, "PopupLabel" );
		m_label_yourname->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_label_yourname->SetLeft( 12 );
		m_label_yourname->SetTop( 5 );
		m_label_yourname->SetText( "Enter your name:" );
	m_section->AddChild( m_label_yourname );

	NEW( m_input_yourname, Input, "PopupInput" );
		m_input_yourname->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_input_yourname->SetLeft( 144 );
		m_input_yourname->SetTop( 5 );
		m_input_yourname->SetRight( 12 );
		m_input_yourname->SetValue( "" );
		m_input_yourname->SetMaxLength( 20 ); // TODO: determine by rendered width
	m_section->AddChild( m_input_yourname );

	NEW( m_label_gameip, Label, "PopupLabel" );
		m_label_gameip->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_label_gameip->SetLeft( 12 );
		m_label_gameip->SetTop( 34 );
		m_label_gameip->SetText( "Enter game IP:" );
	m_section->AddChild( m_label_gameip );
	
	NEW( m_input_gameip, Input, "PopupInput" );
		m_input_gameip->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_input_gameip->SetLeft( 144 );
		m_input_gameip->SetTop( 34 );
		m_input_gameip->SetRight( 12 );
		m_input_gameip->SetValue( "127.0.0.1" );
		m_input_gameip->SetMaxLength( 20 ); // TODO: determine by rendered width
	m_section->AddChild( m_input_gameip );

}

void Join::Hide() {	
		m_section->RemoveChild( m_label_yourname );
		m_section->RemoveChild( m_input_yourname );
		m_section->RemoveChild( m_label_gameip );
		m_section->RemoveChild( m_input_gameip );
	m_body->RemoveChild( m_section );
	
	PopupMenu::Hide();
}

const string Join::GetChoice() const {
	return m_input_yourname->GetValue() + ";" + m_input_gameip->GetValue(); // HACK
}

void Join::SetChoice( const string& choice ) {
	// HACK
	size_t pos = choice.find( ";" );
	ASSERT( pos >= 0, "; not found in choice" );
	m_input_yourname->SetValue( choice.substr( 0, pos ) );
	m_input_gameip->SetValue( choice.substr( pos + 1 ) );
}


void Join::OnNext() {
	
	auto* loader = g_engine->GetUI()->GetLoader();
	if ( loader->IsRunning() ) {
		return;
	}

	m_mainmenu->m_settings.local.player_name = m_input_yourname->GetValue();
	m_mainmenu->m_settings.local.remote_address = m_input_gameip->GetValue();
	
	if ( m_mainmenu->m_settings.local.player_name.empty() ) {
		MenuError( "Please enter your name." );
	}
	else if ( m_mainmenu->m_settings.local.remote_address.empty() ) {
		MenuError( "Player enter game IP." );
	}
	else {

		auto* network = g_engine->GetNetwork();

		loader->SetOnStart( LH( this, network ) {
			m_mt_id = network->MT_Connect( network::CM_CLIENT, m_mainmenu->m_settings.local.remote_address );
			return true;
		});
		loader->SetOnIterate( LH( this, network ) {
			loader->SetLoadingText( "Connecting to " + m_mainmenu->m_settings.local.remote_address + loader->GetDots() );
			auto result = network->MT_GetResult( m_mt_id );
			switch ( result.result ) {
				case network::R_ERROR: {
					Show();
					MenuError( result.message );
					return false;
				}
				case network::R_SUCCESS: {
					Show();
					NEWV( menu, Lobby, m_mainmenu );
					NextMenu( menu );
					return false;
				}
			} 
			return true;
		});
		loader->SetOnStop( LH( this, network ) {
			network->MT_Cancel( m_mt_id );
			network->MT_Disconnect();
			Show();
			return true;
		});
		
		loader->Start();

		Hide();
	}

}

}
}
