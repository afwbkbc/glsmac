#pragma once

#include "Actor.h"

#include "types/Font.h"
#include "types/Vec2.h"
#include "types/Color.h"

using namespace std;
using namespace types;

namespace scene {
namespace actor {

class TextActor : public Actor {
public:
	TextActor( Font *font, const string &text, Color color = { 1.0, 1.0, 1.0, 0.0 } );
	~TextActor();
	
	Font *GetFont() const;
	const string & GetText() const;
	const Color & GetColor() const;
	
	void SetText( const string& text );
	void SetColor( const Color& color );
	
protected:
	types::Font *m_font;
	string m_text;
	types::Color m_color;
};

} /* namespace actor */
} /* namespace scene */
