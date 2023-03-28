#include "Confirmation.h"

namespace ui {
namespace popup {

Confirmation::Confirmation( const std::string class_name )
	: ::ui::object::Popup( class_name )
{
	// TODO: determine size from text
	SetWidth( 520 );
	SetHeight( 120 );
	
	// TODO: fix z index bugs
	SetZIndex( 0.9f );
}

void Confirmation::Create() {
	::ui::object::Popup::Create();
	
	NEW( m_body, Section, SubClass( "Window" ) );
		m_body->SetTitleText( "Confirmation" );
	AddChild( m_body );
	
	NEW( m_label, ::ui::object::Label, SubClass( "Text" ) );
		if ( !m_text.empty() ) {
			m_label->SetText( m_text );
		}
		m_label->SetAlign( UIObject::ALIGN_HCENTER | UIObject::ALIGN_TOP );
		m_label->SetTop( 20 ); // TODO: style?
	m_body->AddChild( m_label );
	
	// TODO: merge with OkCancelPopup ?
	NEW( m_buttons.ok, ::ui::object::Button, SubClass( "ButtonOkCancel" ) );
		m_buttons.ok->SetAlign( ::ui::object::UIObject::ALIGN_LEFT | ::ui::object::UIObject::ALIGN_BOTTOM );
		m_buttons.ok->SetLeft( 12 );
		m_buttons.ok->SetBottom( 6 );
		m_buttons.ok->SetLabel( "YES" );
		m_buttons.ok->SetZIndex( 0.9f ); // TODO: hack
		m_buttons.ok->On( ::ui::UIEvent::EV_BUTTON_CLICK, EH( this ) {
			Confirm();
			return true;
		});
	m_body->AddChild( m_buttons.ok );

	NEW( m_buttons.cancel, ::ui::object::Button, SubClass( "ButtonOkCancel" ) );
		m_buttons.cancel->SetAlign( ::ui::object::UIObject::ALIGN_RIGHT | ::ui::object::UIObject::ALIGN_BOTTOM );
		m_buttons.cancel->SetRight( 12 );
		m_buttons.cancel->SetBottom( 6 );
		m_buttons.cancel->SetLabel( "NO" );
		m_buttons.cancel->SetZIndex( 0.9f ); // TODO: hack
		m_buttons.cancel->On( ::ui::UIEvent::EV_BUTTON_CLICK, EH( this ) {
			Close();
			return true;
		});
	m_body->AddChild( m_buttons.cancel );
	
	On( UIEvent::EV_KEY_DOWN, EH( this ) {
		if ( !data->key.modifiers && data->key.code == UIEvent::K_ENTER ) {
			Confirm();
			return true;
		}
		else {
			return false;
		}
	});
}

void Confirmation::Destroy() {
	
	m_body->RemoveChild( m_label );
	
	m_body->RemoveChild( m_buttons.ok );
	m_body->RemoveChild( m_buttons.cancel );
	
	RemoveChild( m_body );
	
	::ui::object::Popup::Destroy();
}

void Confirmation::SetText( const std::string& text ) {
	if ( text != m_text ) {
		m_text = text;
		if ( m_label ) {
			m_label->SetText( m_text );
		}
	}
}

void Confirmation::Confirm() {
	UIEvent::event_data_t d = {};
	Trigger( UIEvent::EV_CONFIRM, &d );
	Close();
}

}
}
