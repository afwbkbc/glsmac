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
	void UpdatePosition();
	
	void Redraw();
	
protected:
	
	void UpdateText();
	
	types::Font *m_font = nullptr;
	string m_text = "";
	types::Color m_color = { 0.0, 0.0, 0.0, 0.0 };
};

} /* namespace actor */
} /* namespace scene */
