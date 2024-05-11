#include "Text.h"

#include "types/Font.h"
#include "base/ObjectLink.h"
#include "graphics/opengl/actor/Text.h"

namespace scene {
namespace actor {

Text::Text( types::Font* font, const std::string& text, types::Color color )
	:
	Actor( Actor::TYPE_TEXT, "Text" )
	, m_font( font )
	, m_text( text )
	, m_color( color ) {
	m_render_flags |= actor::Actor::RF_USE_2D_POSITION;
}

Text::~Text() {

}

types::Font* Text::GetFont() const {
	return m_font;
}

const std::string& Text::GetText() const {
	return m_text;
}

const types::Color& Text::GetColor() const {
	return m_color;
}

void Text::SetText( const std::string& text ) {
	if ( text != m_text ) {
		m_text = text;

		Redraw();
	}
}

void Text::SetColor( const types::Color& color ) {
	if ( color != m_color ) {
		m_color = color;

		Redraw();
	}
}

void Text::SetFont( types::Font* font ) {
	if ( font != m_font ) {
		m_font = font;

		Redraw();
	}
}

void Text::UpdatePosition() {
	Actor::UpdatePosition();

	Redraw();
}

void Text::Redraw() {
	if ( m_graphics_object ) {
		ASSERT( !m_graphics_object->Removed(), "textactor graphics object removed" );

		auto* gl_actor = m_graphics_object->GetDstObject< graphics::opengl::Text >();
		gl_actor->Update( m_font, m_text, m_position.x, m_position.y );
	}
}

}
}
