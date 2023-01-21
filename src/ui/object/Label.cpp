#include "Label.h"

#include "engine/Engine.h"

using namespace types;

namespace ui {
namespace object {

Label::Label( const string& class_name )
	: UIObject( class_name )
{
	SetAlign( ALIGN_CENTER );
}

void Label::SetText( const string& text ) {
	if ( m_text != text ) {
		m_text = text;
		Realign();
		if ( m_actor ) {
			m_actor->SetText( text );
		}
	}
}

void Label::SetTextColor( const Color& color ) {
	if ( m_color != color ) {
		m_color = color;
		if ( m_actor ) {
			m_actor->SetColor( color );
		}
	}
}

void Label::SetFont( Font* font ) {
	if ( m_font != font ) {
		m_font = font;
		Realign();
		if ( m_actor ) {
			m_actor->SetFont( m_font );
		}
	}
}

void Label::Create() {
	UIObject::Create();
	
	if ( !m_actor ) {
		NEW( m_actor, scene::actor::TextActor, m_font, m_text, m_color );
		AddActor( m_actor );
	}
}

void Label::Destroy() {

	if ( m_actor ) {
		RemoveActor(m_actor);
		Log( "Deleting text actor " + m_actor->GetName() );
		DELETE( m_actor );
		m_actor = nullptr;
	}
	
	UIObject::Destroy();
}

void Label::Align() {
	UIObject::Align();

	if ( m_actor && m_font ) {
		size_t xpos = 0;
		size_t ypos = 0;

		if ( ( m_align & ALIGN_HCENTER ) == ALIGN_HCENTER ) {
			xpos = ( m_object_area.left + m_object_area.right ) / 2 - m_font->GetTextWidth(m_text.c_str()) / 2;
		}
		else if ( m_align & ALIGN_LEFT ) {
			xpos = m_object_area.left;
		}
		else if ( m_align & ALIGN_RIGHT ) {
			xpos = m_object_area.right - m_font->GetTextWidth(m_text.c_str());
		}

		if ( ( m_align & ALIGN_VCENTER ) == ALIGN_VCENTER ) {
			ypos = ( m_object_area.top + m_object_area.bottom ) / 2 + m_font->GetTextHeight(m_text.c_str()) / 2;
		}
		else if ( m_align & ALIGN_TOP ) {
			ypos = m_object_area.top + m_font->GetTextHeight(m_text.c_str());
		}
		else if ( m_align & ALIGN_BOTTOM ) {
			ypos = m_object_area.bottom;
		}

		m_actor->SetPosition({
			ClampX( xpos ),
			ClampY( ypos ),
			-1.0,
		});
		
	}
}

void Label::ApplyStyle() {
	UIObject::ApplyStyle();
	
	Log( "Mouse Text ApplyStyle " + GetStyleModifiersString() );

	if ( Has( Style::A_TEXTCOLOR ) ) {
		SetTextColor( GetColor( Style::A_TEXTCOLOR ) );
	}
	if ( Has( Style::A_FONT ) ) {
		SetFont( (Font*)GetObject( Style::A_FONT ) );
	}
}

}
}
