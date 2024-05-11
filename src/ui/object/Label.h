#pragma once

#include <string>

#include "UIObject.h"

#include "types/Color.h"

namespace types {
class Font;
}

namespace scene::actor {
class Text;
}

namespace ui {
namespace object {

CLASS( Label, UIObject )

	Label( const std::string& class_name = "" );

	void SetText( const std::string& text );
	void SetTextColor( const types::Color& color );
	void SetFont( types::Font* font );

	const std::string& GetText() const;
	const std::string* GetTextPtr() const; // be careful

	void Create() override;
	void Destroy() override;
	void Align() override;

protected:

	void ApplyStyle() override;

	types::Font* m_font = nullptr;
	std::string m_text = "";
	size_t m_text_width = 0;
	size_t m_text_height = 0;
	types::Color m_color = {
		0.0,
		0.0,
		0.0,
		0.0
	};

	scene::actor::Text* m_actor = nullptr;

	void UpdateTextDimensions();
};

}
}
