#include "Section.h"

#include "Label.h"
#include "Panel.h"

namespace ui_legacy {
namespace object {

Section::Section( const std::string& class_name )
	: UIContainer( class_name ) {
	//
}

void Section::SetTitleText( const std::string& title_text ) {
	if ( title_text != m_title_text ) {
		m_title_text = title_text;
		if ( m_titlelabel ) {
			m_titlelabel->SetText( title_text );
		}
	}
}

void Section::AddChild( UIObject* object ) {
	m_body->AddChild( object );
}

void Section::RemoveChild( UIObject* object ) {
	m_body->RemoveChild( object );
}

void Section::Create() {
	UIContainer::Create();

	NEW( m_frame, Panel );
	m_frame->SetAlign( ALIGN_CENTER );
	m_frame->SetMargin( 3 );
	m_frame->ForwardStyleAttributesM( m_forwarded_style_attributes );
	UIContainer::AddChild( m_frame );

	if ( !m_title_text.empty() ) {
		NEW( m_titlebar, Panel );
		m_titlebar->SetAlign( ALIGN_TOP );
		m_titlebar->SetMargin(
			{
				3, // left
				3, // top
				3, // right
				-3 // bottom
			}
		);
		m_titlebar->ForwardStyleAttributesM( m_forwarded_header_style_attributes );
		m_frame->AddChild( m_titlebar );
		NEW( m_titlelabel, Label );
		m_titlelabel->SetText( m_title_text );
		m_titlelabel->SetMargin( 2 );
		m_titlelabel->ForwardStyleAttributesM( m_forwarded_header_label_style_attributes );
		m_titlebar->AddChild( m_titlelabel );
	}

	NEW( m_body, Panel );
	m_frame->AddChild( m_body );
}

void Section::Destroy() {

	m_frame->RemoveChild( m_body );
	if ( m_titlebar ) {
		m_titlebar->RemoveChild( m_titlelabel );
		m_frame->RemoveChild( m_titlebar );
	}
	UIContainer::RemoveChild( m_frame );

	UIContainer::Destroy();
}

void Section::ApplyStyle() {

	if ( m_body ) {
		if ( Has( A_HEADER_HEIGHT ) && !m_title_text.empty() ) {
			m_body->SetTop( Get( A_HEADER_HEIGHT ) + 2 );
		}
		else {
			m_body->SetTop( 0 );
		}
	}

	UIContainer::ApplyStyle();
}

}
}
