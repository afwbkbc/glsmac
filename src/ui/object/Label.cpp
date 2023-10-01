#include "Label.h"

#include "engine/Engine.h"

using namespace types;

namespace ui {
namespace object {

Label::Label( const std::string& class_name )
	: UIObject( class_name ) {
	SetAlign( ALIGN_CENTER );
}

void Label::SetText( const std::string& text ) {
	if ( m_text != text ) {
		m_text = text;
		if ( m_font ) {
			UpdateTextDimensions();
		}
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
		UpdateTextDimensions();
		Realign();
		if ( m_actor ) {
			m_actor->SetFont( m_font );
		}
	}
}

const std::string& Label::GetText() const {
	return m_text;
}

const std::string* Label::GetTextPtr() const {
	return &m_text;
}

void Label::Create() {
	UIObject::Create();

	if ( !m_actor ) {
		NEW( m_actor, scene::actor::Text, m_font, m_text, m_color );
		AddActor( m_actor );
	}
}

void Label::Destroy() {

	if ( m_actor ) {
		RemoveActor( m_actor );
		DELETE( m_actor );
		m_actor = nullptr;
	}

	UIObject::Destroy();
}

void Label::Align() {
	UIObject::Align();

	if ( m_actor && m_font ) {
		ssize_t xpos = 0;
		ssize_t ypos = 0;

		if ( ( m_align & ALIGN_HCENTER ) == ALIGN_HCENTER ) {
			xpos = ( m_object_area.left + m_object_area.right ) / 2 - m_text_width / 2;
		}
		else if ( m_align & ALIGN_LEFT ) {
			xpos = m_object_area.left;
		}
		else if ( m_align & ALIGN_RIGHT ) {
			xpos = m_object_area.right - m_text_width;
		}

		if ( ( m_align & ALIGN_VCENTER ) == ALIGN_VCENTER ) {
			ypos = ( m_object_area.top + m_object_area.bottom ) / 2 + m_text_height / 2;
		}
		else if ( m_align & ALIGN_TOP ) {
			ypos = m_object_area.top + m_text_height;
		}
		else if ( m_align & ALIGN_BOTTOM ) {
			ypos = m_object_area.bottom;
		}

		m_actor->SetPosition(
			{
				ClampX( xpos ),
				ClampY( ypos ),
				m_absolute_z_index,
			}
		);
	}
}

void Label::ApplyStyle() {
	UIObject::ApplyStyle();

	if ( Has( Style::A_TEXT_ALIGN ) ) {
		SetAlign( Get( Style::A_TEXT_ALIGN ) );
	}
	if ( Has( Style::A_TEXT_COLOR ) ) {
		SetTextColor( GetColor( Style::A_TEXT_COLOR ) );
	}
	if ( Has( Style::A_FONT ) ) {
		SetFont( (Font*)GetObject( Style::A_FONT ) );
	}
}

void Label::UpdateTextDimensions() {
	m_text_width = m_font->GetTextWidth( m_text.c_str() );
	m_text_height = m_font->GetTextHeight( m_text.c_str() );

	if ( !m_size.width ) {
		SetWidth( m_text_width );
	}
	if ( !m_size.height ) {
		SetHeight( m_text_height );
	}
}

}
}
