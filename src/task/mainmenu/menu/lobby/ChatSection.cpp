#include "ChatSection.h"

#include "Lobby.h"
#include "ui_legacy/object/TextView.h"
#include "ui_legacy/object/Input.h"

namespace task {
namespace mainmenu {
namespace lobby {

ChatSection::ChatSection( Lobby* lobby )
	: LobbySection( lobby ) {
	SetAlign( ui_legacy::ALIGN_RIGHT | ui_legacy::ALIGN_TOP );
}

void ChatSection::Create() {
	LobbySection::Create();

	NEW( m_log, ::ui_legacy::object::TextView, "PopupTextList" );
	m_log->SetAlign( ui_legacy::ALIGN_TOP );
	m_log->SetBottom( 23 );
	m_log->SetLeft( 2 );
	m_log->SetRight( 2 );
	m_log->SetTop( 2 );
	AddChild( m_log );

	NEW( m_input, ::ui_legacy::object::Input, "PopupInput" );
	m_input->SetAlign( ui_legacy::ALIGN_BOTTOM );
	m_input->SetHeight( 20 );
	m_input->SetMaxLength( 40 ); // TODO: infinite length
	m_input->On(
		ui_legacy::event::EV_KEY_DOWN, EH( this ) {
			if ( data->key.code == ui_legacy::event::K_ENTER ) {
				const auto& value = m_input->GetValue();
				if ( !value.empty() ) {
					GetLobby()->Message( value );
					m_input->Clear();
				}
				return true;
			}
			return false;
		}
	);
	AddChild( m_input );
}

void ChatSection::Align() {
	SetTop( 208 );
	SetWidth( 496 );
	SetHeight( 156 );

	LobbySection::Align();
}

void ChatSection::Destroy() {
	RemoveChild( m_log );
	m_log = nullptr;
	RemoveChild( m_input );
	m_input = nullptr;

	LobbySection::Destroy();
}

void ChatSection::AddMessage( const std::string& message ) {
	if ( m_log ) {
		m_log->AddLine( message );
		m_log->ScrollToEnd();
	}
}

}
}
}
