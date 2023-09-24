#pragma once

#include "Actor.h"

#include "types/Font.h"
#include "types/Vec2.h"
#include "types/Color.h"

using namespace types;

namespace scene {
namespace actor {

CLASS( Text, Actor )

	Text(
		Font* font, const std::string& text, Color color = {
		1.0,
		1.0,
		1.0,
		0.0
	}
	);
	~Text();

	Font* GetFont() const;
	const std::string& GetText() const;
	const Color& GetColor() const;

	void SetText( const std::string& text );
	void SetColor( const Color& color );
	void SetFont( Font* font );
	void UpdatePosition() override;

	void Redraw();

protected:

	void UpdateText();

	types::Font* m_font = nullptr;
	std::string m_text = "";
	types::Color m_color = {
		0.0,
		0.0,
		0.0,
		0.0
	};
};

} /* namespace actor */
} /* namespace scene */
