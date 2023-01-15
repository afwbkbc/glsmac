#include "TextActor.h"

using namespace std;
using namespace types;

#include "renderer/opengl/TextActor.h"

namespace scene {
namespace actor {

TextActor::TextActor( types::Font *font, const string &text, types::Color color ) :
	Actor( Actor::TYPE_TEXT, "Text" ),
	m_font( font ),
	m_text( text ),
	m_color( color )
{

}

TextActor::~TextActor() {

}

Font *TextActor::GetFont() const {
	return m_font;
}

const string& TextActor::GetText() const {
	return m_text;
}

const Color& TextActor::GetColor() const {
	return m_color;
}

void TextActor::SetText( const string& text ) {
	if ( text != m_text ) {
		m_text = text;

		Redraw();
	}
}

void TextActor::SetColor( const Color& color ) {
	if ( color != m_color ) {
		m_color = color;

		Redraw();
	}
}

void TextActor::UpdatePosition() {
	Actor::UpdatePosition();
	
	Redraw();
}

void TextActor::Redraw() {
	//Log("UPDATETEXT");
	if ( m_renderer_object ) {
		auto* gl_actor = m_renderer_object->GetDstObject<renderer::opengl::TextActor>();
		gl_actor->Update( m_text, m_position.x, m_position.y );
	}
}

} /* namespace actor */
} /* namespace scene */
