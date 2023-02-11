#include "Host.h"

#include "engine/Engine.h"

#include "types/ThreadCall.h"

#include "Lobby.h"

namespace game {
namespace mainmenu {

Host::Host( MainMenu* mainmenu ) : PopupMenu( mainmenu, "HOST MULTIPLAYER GAME" ) {
	SetWidth( 500 );
	SetHeight( 148 );
	SetFlags( { PF_HAS_OK, PF_HAS_CANCEL } );
}

Host::~Host() {
}

void Host::Show() {
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

	NEW( m_label_gamename, Label, "PopupLabel" );
		m_label_gamename->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_label_gamename->SetLeft( 12 );
		m_label_gamename->SetTop( 34 );
		m_label_gamename->SetText( "Enter game name:" );
	m_section->AddChild( m_label_gamename );
	
	NEW( m_input_gamename, Input, "PopupInput" );
		m_input_gamename->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_input_gamename->SetLeft( 144 );
		m_input_gamename->SetTop( 34 );
		m_input_gamename->SetRight( 12 );
		m_input_gamename->SetValue( "" );
		m_input_gamename->SetMaxLength( 20 ); // TODO: determine by rendered width
	m_section->AddChild( m_input_gamename );

}

void Host::Hide() {	
		m_section->RemoveChild( m_label_yourname );
		m_section->RemoveChild( m_input_yourname );
		m_section->RemoveChild( m_label_gamename );
		m_section->RemoveChild( m_input_gamename );
	m_body->RemoveChild( m_section );
	
	PopupMenu::Hide();
}

const std::string Host::GetChoice() const {
	return m_input_yourname->GetValue() + ";" + m_input_gamename->GetValue(); // HACK
}

void Host::SetChoice( const std::string& choice ) {
	// HACK
	size_t pos = choice.find( ";" );
	ASSERT( pos >= 0, "; not found in choice" );
	m_input_yourname->SetValue( choice.substr( 0, pos ) );
	m_input_gamename->SetValue( choice.substr( pos + 1 ) );
}


void Host::OnNext() {
	
	auto* loader = g_engine->GetUI()->GetLoader();
	if ( loader->IsRunning() ) {
		return;
	}

	m_mainmenu->m_settings.local.player_name = m_input_yourname->GetValue();
	m_mainmenu->m_settings.local.remote_address = "";
	m_mainmenu->m_settings.global.game_name = m_input_gamename->GetValue();
	
	if ( m_mainmenu->m_settings.local.player_name.empty() ) {
		MenuError( "Please enter your name." );
	}
	else if ( m_mainmenu->m_settings.global.game_name.empty() ) {
		MenuError( "Player enter game name." );
	}
	else {

		auto* network = g_engine->GetNetwork();

		loader->SetOnStart( LH( this, network ) {
			m_mt_id = network->MT_Connect( network::CM_SERVER );
			return true;
		});
		loader->SetOnIterate( LH( this, network ) {
			loader->SetLoadingText( "Creating game" + loader->GetDots() );
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
