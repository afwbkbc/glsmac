#include "Error.h"

#include "ui/object/Section.h"
#include "ui/object/Button.h"
#include "ui/object/Label.h"

namespace ui {
namespace popup {

Error::Error( const std::string class_name )
	: object::Popup( class_name ) {
	// TODO: determine size from text
	SetWidth( 560 );
	SetHeight( 120 );

	// TODO: fix z index bugs
	SetZIndex( 0.9f );
}

void Error::Create() {
	object::Popup::Create();

	NEW( m_body, object::Section, SubClass( "Window" ) );
	m_body->SetTitleText( "Error" );
	AddChild( m_body );

	NEW( m_label, object::Label, SubClass( "Text" ) );
	if ( !m_text.empty() ) {
		m_label->SetText( m_text );
	}
	m_label->SetAlign( ALIGN_HCENTER | ALIGN_TOP );
	m_label->SetTop( 20 ); // TODO: style?
	m_body->AddChild( m_label );

	NEW( m_ok_button, object::Button, SubClass( "ButtonOkCancel" ) );
	m_ok_button->SetAlign( ALIGN_HCENTER | ALIGN_BOTTOM );
	m_ok_button->SetBottom( 6 );
	m_ok_button->SetLabel( "OK" );
	m_ok_button->SetZIndex( 0.9f ); // TODO: hack
	m_ok_button->On(
		ui::event::EV_BUTTON_CLICK, EH( this ) {
			Close();
			return true;
		}
	);
	m_body->AddChild( m_ok_button );

	On(
		ui::event::EV_KEY_DOWN, EH( this ) {
			if ( !data->key.modifiers ) {
				if ( data->key.code == ui::event::K_ENTER || ( data->key.code == ui::event::K_ESCAPE ) ) {
					Close();
				}
			}
			return true;
		}
	);
}

void Error::Destroy() {

	m_body->RemoveChild( m_label );

	m_body->RemoveChild( m_ok_button );

	RemoveChild( m_body );

	object::Popup::Destroy();
}

void Error::SetText( const std::string& text ) {
	if ( text != m_text ) {
		m_text = text;
		if ( m_label ) {
			m_label->SetText( m_text );
		}
	}
}

}
}
