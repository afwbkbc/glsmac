#pragma once

#include "Actor.h"

#include "types/Font.h"
#include "types/Vec2.h"
#include "types/Color.h"

using namespace types;

namespace scene {
namespace actor {

class TextActor : public Actor {
public:
	TextActor( Font *font, const std::string &text, Color color = Color::WHITE() );
	~TextActor();
	
	Font *GetFont() const;
	const std::string & GetText() const;
	const Color & GetColor() const;
	
	void SetColor( const Color& color );
	
protected:
	types::Font *m_font;
	const std::string m_text;
	types::Color m_color;
};

} /* namespace actor */
} /* namespace scene */
