#include "ChatPopup.h"

#include "../../Game.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

ChatPopup::ChatPopup( Game* game )
	: OkCancelPopup( game ) {
	SetTitleText( "COMMUNICATIONS OFFICER" );
	SetWidth( 630 );
	SetHeight( 300 );
}

void ChatPopup::Create() {
	OkCancelPopup::Create();

	NEW( m_message_section, ::ui::object::Section, SubClass( "Frame" ) );
	m_message_section->SetAlign( UIObject::ALIGN_TOP );
	m_message_section->SetLeft( 6 );
	m_message_section->SetRight( 6 );
	m_message_section->SetTop( 34 );
	m_message_section->SetHeight( 34 );
	AddChild( m_message_section );

	NEW( m_message_label, ::ui::object::Label, SubClass( "Label" ) );
	m_message_label->SetText( "Message:" );
	m_message_label->SetTop( 4 );
	m_message_label->SetLeft( 6 );
	m_message_section->AddChild( m_message_label );

	NEW( m_message_input, ::ui::object::Input, "PopupInput" );
	m_message_input->SetMaxLength( 50 ); // TODO: infinite length
	m_message_input->SetTop( 4 );
	m_message_input->SetLeft( 80 );
	m_message_input->SetRight( 6 );
	m_message_input->On(
		UIEvent::EV_KEY_DOWN, EH( this ) {
			if ( !data->key.modifiers ) {
				if ( data->key.code == UIEvent::K_ENTER ) {
					OnOk();
					return true;
				}
			}
			return false;
		}
	);
	m_message_section->AddChild( m_message_input );
}

void ChatPopup::Destroy() {

	m_message_section->RemoveChild( m_message_label );
	m_message_section->RemoveChild( m_message_input );
	RemoveChild( m_message_section );

	OkCancelPopup::Destroy();
}

void ChatPopup::OnOk() {
	const auto& message = m_message_input->GetValue();
	if ( !message.empty() ) {
		m_game->SendChatMessage( message );
		m_message_input->Clear();
		Close();
	}
}

}
}
}
}
