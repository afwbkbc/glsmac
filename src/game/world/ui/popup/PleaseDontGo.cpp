#include "PleaseDontGo.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

PleaseDontGo::PleaseDontGo( World* world, const ui_handler_t on_order )
	: Popup( world )
	, m_on_order( on_order )
{
	SetTitleText( "SID MEIER'S ALPHA CENTAURI" );
	SetWidth( 630 );
	SetHeight( 300 );
}

void PleaseDontGo::Create() {
	Popup::Create();
	
	NEW( m_image, ::ui::object::Surface, SubClass( "PleaseDontGoImage" ) );
	AddChild( m_image );
	
	NEW( m_text, ::ui::object::Label, SubClass( "PleaseDontGoText" ) );
		m_text->SetText( "Do you really want to quit?" );
	AddChild( m_text );
	
	NEW( m_choices, ::ui::object::ChoiceList, "PopupButtonList" );
		m_choices->SetZIndex( 0.6f ); // TODO: fix z index bugs
		m_choices->SetTop( 164 );
		m_choices->SetMargin( 3 );
		m_choices->SetChoices({
			"Oops, no, wait!",
			"Yes, that's an order mister!"
		});
		m_choices->On( UIEvent::EV_SELECT, EH( this ) {
			SelectChoice();
			return true;
		});
	AddChild( m_choices );
	
	NEW( m_sound, SoundEffect, SubClass( "PleaseDontGoSound" ) );
	AddChild( m_sound );
}

void PleaseDontGo::Destroy() {
	
	RemoveChild( m_image );
	RemoveChild( m_text );
	RemoveChild( m_choices );
	RemoveChild( m_sound );
	
	Popup::Destroy();
}

void PleaseDontGo::SelectChoice() {
	if ( m_choices->GetValue() == "Yes, that's an order mister!" ) {
		m_on_order();
	}
	Close();
}

}
}
}
}
