#include "Buttons.h"

#include "ui/object/Button.h"

#include "BasePopup.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {

Buttons::Buttons( BasePopup* popup )
	: BPSection( popup, "Buttons" ) {}

void Buttons::Create() {
	BPSection::Create();

	NEW( m_buttons.rename, ::ui::object::Button, "PopupButtonOkCancel" );
	m_buttons.rename->SetAlign( ::ui::ALIGN_LEFT | ::ui::ALIGN_VCENTER );
	m_buttons.rename->SetLeft( 3 );
	m_buttons.rename->SetRight( 2 );
	m_buttons.rename->SetLabel( "RENAME" );
	m_buttons.rename->On(
		::ui::event::EV_BUTTON_CLICK, EH( this ) {
			Log( "TODO: RENAME" );
			return true;
		}
	);
	AddChild( m_buttons.rename );

	NEW( m_buttons.ok, ::ui::object::Button, "PopupButtonOkCancel" );
	m_buttons.ok->SetAlign( ::ui::ALIGN_RIGHT | ::ui::ALIGN_VCENTER );
	m_buttons.ok->SetLeft( 2 );
	m_buttons.ok->SetRight( 3 );
	m_buttons.ok->SetLabel( "OK" );
	m_buttons.ok->On(
		::ui::event::EV_BUTTON_CLICK, EH( this ) {
			m_popup->Close();
			return true;
		}
	);
	AddChild( m_buttons.ok );

}

void Buttons::Destroy() {

	RemoveChild( m_buttons.rename );
	RemoveChild( m_buttons.ok );

	BPSection::Destroy();
}

}
}
}
}
}
