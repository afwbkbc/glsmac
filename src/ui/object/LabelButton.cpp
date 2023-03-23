#include "LabelButton.h"

#define DOUBLECLICK_MAX_MS 1000

namespace ui {
namespace object {

LabelButton::LabelButton( const std::string& class_name )
	: SimpleButton( class_name )
{	
	//
}

void LabelButton::Create() {
	SimpleButton::Create();
	
	NEW( m_label, Label );
		m_label->SetText( m_label_text );
		//m_label->SetMargin( 3 );
		m_label->ForwardStyleAttributesV({
			Style::A_FONT,
			Style::A_TEXT_COLOR,
		});
		m_label->ForwardStyleAttribute( Style::A_TEXT_ALIGN, Style::A_ALIGN );
		m_label->ForwardStyleAttribute( Style::A_TEXT_LEFT, Style::A_LEFT );
	AddChild( m_label );
}

void LabelButton::Destroy() {
	RemoveChild( m_label );
	
	SimpleButton::Destroy();
}

void LabelButton::SetLabel( const std::string& text ) {
	if ( text != m_label_text ) {
		m_label_text = text;
		if ( m_label ) {
			m_label->SetText( m_label_text );
		}
	}
}

const std::string& LabelButton::GetLabel() const {
	return m_label_text;
}

}
}
