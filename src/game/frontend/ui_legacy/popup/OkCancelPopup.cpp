#include "OkCancelPopup.h"

#include "ui_legacy/object/Button.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {

OkCancelPopup::OkCancelPopup( Game* game )
	: Popup( game ) {

}

void OkCancelPopup::Create() {
	Popup::Create();

	NEW( m_buttons.ok, ::ui_legacy::object::Button, "PopupButtonOkCancel" );
	m_buttons.ok->SetAlign( ::ui_legacy::ALIGN_LEFT | ::ui_legacy::ALIGN_BOTTOM );
	m_buttons.ok->SetLeft( 12 );
	m_buttons.ok->SetBottom( 40 );
	m_buttons.ok->SetLabel( "OK" );
	m_buttons.ok->SetZIndex( 0.9f ); // TODO: hack
	m_buttons.ok->On(
		::ui_legacy::event::EV_BUTTON_CLICK, EH( this ) {
			OnOk();
			return true;
		}
	);
	AddChild( m_buttons.ok );

	NEW( m_buttons.cancel, ::ui_legacy::object::Button, "PopupButtonOkCancel" );
	m_buttons.cancel->SetAlign( ::ui_legacy::ALIGN_RIGHT | ::ui_legacy::ALIGN_BOTTOM );
	m_buttons.cancel->SetRight( 12 );
	m_buttons.cancel->SetBottom( 40 );
	m_buttons.cancel->SetLabel( "CANCEL" );
	m_buttons.cancel->SetZIndex( 0.9f ); // TODO: hack
	m_buttons.cancel->On(
		::ui_legacy::event::EV_BUTTON_CLICK, EH( this ) {
			Close();
			return true;
		}
	);
	AddChild( m_buttons.cancel );
}

void OkCancelPopup::Align() {
	Popup::Align();

	const auto wh = GetWidth() / 2;
	if ( m_buttons.ok ) {
		m_buttons.ok->SetWidth( wh - m_buttons.ok->GetLeft() * 1.5f );
	}
	if ( m_buttons.cancel ) {
		m_buttons.cancel->SetWidth( wh - m_buttons.cancel->GetRight() * 1.5f );
	}
}

void OkCancelPopup::Destroy() {
	RemoveChild( m_buttons.ok );
	RemoveChild( m_buttons.cancel );

	Popup::Destroy();
}

}
}
}
}
