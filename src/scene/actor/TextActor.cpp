#include "TextActor.h"

using namespace std;
using namespace types;

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
	m_text = text;
}

void TextActor::SetColor( const Color& color ) {
	m_color = color;
}

} /* namespace actor */
} /* namespace scene */
