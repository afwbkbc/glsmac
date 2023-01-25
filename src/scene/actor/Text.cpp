#include "Text.h"

using namespace std;
using namespace types;

#include "graphics/opengl/actor/Text.h"

namespace scene {
namespace actor {

Text::Text( types::Font *font, const string &text, types::Color color ) :
	Actor( Actor::TYPE_TEXT, "Text" ),
	m_font( font ),
	m_text( text ),
	m_color( color )
{

}

Text::~Text() {

}

Font *Text::GetFont() const {
	return m_font;
}

const string& Text::GetText() const {
	return m_text;
}

const Color& Text::GetColor() const {
	return m_color;
}

void Text::SetText( const string& text ) {
	if ( text != m_text ) {
		m_text = text;

		Redraw();
	}
}

void Text::SetColor( const Color& color ) {
	if ( color != m_color ) {
		m_color = color;

		Redraw();
	}
}

void Text::SetFont( Font* font ) {
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
#ifdef DEBUG
		if ( m_graphics_object->Removed() ) {
			throw runtime_error( "textactor graphics object removed" );
		}
#endif
		auto* gl_actor = m_graphics_object->GetDstObject<graphics::opengl::Text>();
		gl_actor->Update( m_font, m_text, m_position.x, m_position.y );
	}
}

} /* namespace actor */
} /* namespace scene */
