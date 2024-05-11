#include "PleaseDontGo.h"

#include "ui/object/Surface.h"
#include "ui/object/Label.h"
#include "ui/object/SoundEffect.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

PleaseDontGo::PleaseDontGo( Game* game, const ::ui::ui_handler_t on_order )
	: Popup( game )
	, m_on_order( on_order ) {
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

	NEW( m_choices, ::ui::object::NumChoiceList, "PopupButtonList" );
	m_choices->SetZIndex( 0.6f ); // TODO: fix z index bugs
	m_choices->SetTop( 164 );
	m_choices->SetMargin( 3 );
	m_choices->SetChoicesV(
		{
			"Oops, no, wait!",
			"Yes, that's an order mister!"
		}
	);
	m_choices->On(
		::ui::event::EV_SELECT, EH( this ) {
			SelectChoice();
			return true;
		}
	);
	AddChild( m_choices );
	m_choices->SetValue( 2 );

	NEW( m_sound, ::ui::object::SoundEffect, SubClass( "PleaseDontGoSound" ) );
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
	if ( m_choices->GetValueString() == "Yes, that's an order mister!" ) {
		m_on_order();
		CloseNow();
	}
	else {
		Close();
	}
}

}
}
}
}
