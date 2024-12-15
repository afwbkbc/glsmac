#include "ChatPopup.h"

#include "game/frontend/Game.h"
#include "ui_legacy/object/Section.h"
#include "ui_legacy/object/Label.h"
#include "ui_legacy/object/Input.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {

ChatPopup::ChatPopup( Game* game )
	: OkCancelPopup( game ) {
	SetTitleText( "COMMUNICATIONS OFFICER" );
	SetWidth( 630 );
	SetHeight( 300 );
}

void ChatPopup::Create() {
	OkCancelPopup::Create();

	NEW( m_message_section, ::ui_legacy::object::Section, SubClass( "InnerFrame" ) );
	m_message_section->SetAlign( ::ui_legacy::ALIGN_TOP );
	m_message_section->SetLeft( 6 );
	m_message_section->SetRight( 6 );
	m_message_section->SetTop( 34 );
	m_message_section->SetHeight( 36 );
	AddChild( m_message_section );

	NEW( m_message_label, ::ui_legacy::object::Label, SubClass( "Label" ) );
	m_message_label->SetText( "Message:" );
	m_message_label->SetTop( 4 );
	m_message_label->SetLeft( 6 );
	m_message_section->AddChild( m_message_label );

	NEW( m_message_input, ::ui_legacy::object::Input, "PopupInput" );
	m_message_input->SetMaxLength( 50 ); // TODO: infinite length
	m_message_input->SetTop( 4 );
	m_message_input->SetLeft( 80 );
	m_message_input->SetRight( 6 );
	m_message_input->On(
		::ui_legacy::event::EV_KEY_DOWN, EH( this ) {
			if ( !data->key.modifiers ) {
				if ( data->key.code == ::ui_legacy::event::K_ENTER ) {
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
