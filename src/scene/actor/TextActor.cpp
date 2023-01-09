#include "TextActor.h"

namespace scene {
namespace actor {

TextActor::TextActor( types::Font *font, const std::string &text, types::Color color ) :
	Actor( Actor::TYPE_TEXT, "Text" ),
	m_font( font ),
	m_text( text ),
	m_color( color )
{

}

TextActor::~TextActor() {

}

} /* namespace actor */
} /* namespace scene */
