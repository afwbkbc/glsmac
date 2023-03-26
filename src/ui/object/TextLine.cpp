#include "TextLine.h"
#include "engine/Engine.h"

namespace ui {
namespace object {

TextLine::TextLine( const std::string& class_name )
	: UIContainer( class_name )
{
	AddEventContexts( EC_MOUSEMOVE );
}

void TextLine::Create() {
	UIContainer::Create();
	
	NEW( m_background, Surface );
		m_background->ForwardStyleAttributesM( m_forwarded_style_attributes.background );
		m_background->SetZIndex( 0.6f ); // TODO: fix z index bugs
	AddChild( m_background );
	NEW( m_label, Label );
		if ( !m_text.empty() ) {
			m_label->SetText( m_text );
		}
		m_label->ForwardStyleAttributesM( m_forwarded_style_attributes.label );
	AddChild( m_label );
	NEW( m_icon, Surface );
		m_icon->ForwardStyleAttributesM( m_forwarded_style_attributes.icon );
		m_icon->SetZIndex( 0.6f ); // TODO: fix z index bugs
	AddChild( m_icon );
}

void TextLine::Destroy() {
	RemoveChild( m_background );
	RemoveChild( m_label );
	RemoveChild( m_icon );
	
	UIContainer::Destroy();
}

void TextLine::SetText( const std::string& text ) {
	if ( m_text != text ) {
		m_text = text;
		if ( m_label ) {
			m_label->SetText( text );
		}
	}
}

const std::string& TextLine::GetText() const {
	ASSERT( m_label, "label not initialized" );
	return m_label->GetText();
}

const std::string* TextLine::GetTextPtr() const {
	ASSERT( m_label, "label not initialized" );
	return m_label->GetTextPtr();
}

}
}
