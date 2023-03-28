#include "Confirm.h"

namespace ui {
namespace popup {

Confirm::Confirm( const std::string class_name )
	: object::Popup( class_name )
{
	// TODO: determine size from text
	SetWidth( 520 );
	SetHeight( 120 );
	
	// TODO: fix z index bugs
	SetZIndex( 0.9f );
}

void Confirm::Create() {
	object::Popup::Create();
	
	NEW( m_body, Section, SubClass( "Window" ) );
		m_body->SetTitleText( "Confirmation" );
	AddChild( m_body );
	
	NEW( m_label, object::Label, SubClass( "Text" ) );
		if ( !m_text.empty() ) {
			m_label->SetText( m_text );
		}
		m_label->SetAlign( UIObject::ALIGN_HCENTER | UIObject::ALIGN_TOP );
		m_label->SetTop( 20 ); // TODO: style?
	m_body->AddChild( m_label );
	
	NEW( m_buttons.ok, object::Button, SubClass( "ButtonOkCancel" ) );
		m_buttons.ok->SetAlign( object::UIObject::ALIGN_LEFT | object::UIObject::ALIGN_BOTTOM );
		m_buttons.ok->SetLeft( 12 );
		m_buttons.ok->SetBottom( 6 );
		m_buttons.ok->SetLabel( "YES" );
		m_buttons.ok->SetZIndex( 0.9f ); // TODO: hack
		m_buttons.ok->On( UIEvent::EV_BUTTON_CLICK, EH( this ) {
			Confirmed();
			return true;
		});
	m_body->AddChild( m_buttons.ok );

	NEW( m_buttons.cancel, object::Button, SubClass( "ButtonOkCancel" ) );
		m_buttons.cancel->SetAlign( object::UIObject::ALIGN_RIGHT | object::UIObject::ALIGN_BOTTOM );
		m_buttons.cancel->SetRight( 12 );
		m_buttons.cancel->SetBottom( 6 );
		m_buttons.cancel->SetLabel( "NO" );
		m_buttons.cancel->SetZIndex( 0.9f ); // TODO: hack
		m_buttons.cancel->On( UIEvent::EV_BUTTON_CLICK, EH( this ) {
			Close();
			return true;
		});
	m_body->AddChild( m_buttons.cancel );
	
	On( UIEvent::EV_KEY_DOWN, EH( this ) {
		if ( !data->key.modifiers && data->key.code == UIEvent::K_ENTER ) {
			Confirmed();
			return true;
		}
		else {
			return false;
		}
	});
}

void Confirm::Destroy() {
	
	m_body->RemoveChild( m_label );
	
	m_body->RemoveChild( m_buttons.ok );
	m_body->RemoveChild( m_buttons.cancel );
	
	RemoveChild( m_body );
	
	object::Popup::Destroy();
}

void Confirm::SetText( const std::string& text ) {
	if ( text != m_text ) {
		m_text = text;
		if ( m_label ) {
			m_label->SetText( m_text );
		}
	}
}

void Confirm::Confirmed() {
	UIEvent::event_data_t d = {};
	Trigger( UIEvent::EV_CONFIRM, &d );
	Close();
}

}
}
