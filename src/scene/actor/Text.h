#pragma once

#include "Actor.h"

#include "types/Color.h"

namespace types {
class Font;
}

namespace scene {
namespace actor {

CLASS( Text, Actor )

	Text(
		types::Font* font, const std::string& text, types::Color color = {
		1.0,
		1.0,
		1.0,
		0.0
	}
	);
	~Text();

	types::Font* GetFont() const;
	const std::string& GetText() const;
	const types::Color& GetColor() const;

	void SetText( const std::string& text );
	void SetColor( const types::Color& color );
	void SetFont( types::Font* font );
	void UpdatePosition() override;

	void Redraw();

protected:

	types::Font* m_font = nullptr;
	std::string m_text = "";
	types::Color m_color = {
		0.0,
		0.0,
		0.0,
		0.0
	};
};

}
}
