#include "ChatSection.h"

#include "Lobby.h"

namespace task {
namespace mainmenu {
namespace lobby {

ChatSection::ChatSection( Lobby* lobby )
	: LobbySection( lobby )
{
	SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
}

void ChatSection::Create() {
	LobbySection::Create();

	NEW( m_log, ::ui::object::TextView, "PopupTextList" );
		m_log->SetAlign( UIObject::ALIGN_TOP );
		m_log->SetBottom( 23 );
		m_log->SetLeft( 2 );
		m_log->SetRight( 2 );
		m_log->SetTop( 2 );
	AddChild( m_log );

	NEW( m_input, ::ui::object::Input, "PopupInput" )
		m_input->SetAlign( UIObject::ALIGN_BOTTOM );
		m_input->SetHeight( 20 );
		m_input->SetMaxLength( 40 ); // TODO: infinite length
		m_input->On( UIEvent::EV_KEY_DOWN, EH( this ) {
			if ( data->key.code == UIEvent::K_ENTER ) {
				const auto& value = m_input->GetValue();
				if ( !value.empty() ) {
					GetLobby()->Message( value );
					m_input->Clear();
				}
				return true;
			}
			return false;
		});
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
		m_log->ScrollToBottom();
	}
}

}
}
}
