#include "ChatPopup.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

ChatPopup::ChatPopup( Game* game )
	: Popup( game ) {
	SetTitleText( "COMMUNICATIONS OFFICER" );
	SetWidth( 630 );
	SetHeight( 300 );
}

void ChatPopup::Create() {
	Popup::Create();

	NEW( m_text, ::ui::object::Label );
	m_text->SetText( "Message:" );
	AddChild( m_text );

}

void ChatPopup::Destroy() {

	RemoveChild( m_text );

	Popup::Destroy();
}

}
}
}
}
