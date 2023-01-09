#pragma once

#include "Actor.h"

#include "types/Font.h"
#include "types/Vec2.h"
#include "types/Color.h"

namespace scene {
namespace actor {

class TextActor : public Actor {
public:
	TextActor( types::Font *font, const std::string &text, types::Color color = types::Color::WHITE() );
	~TextActor();
	types::Font *GetFont() const {
		return m_font;
	}
	const std::string & GetText() const {
		return m_text;
	}
	const types::Color & GetColor() const {
		return m_color;
	}
protected:
	types::Font *m_font;
	const std::string m_text;
	types::Color m_color;
};

} /* namespace actor */
} /* namespace scene */
