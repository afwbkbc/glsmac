#pragma once

#include "UIContainer.h"

namespace ui {
namespace object {

class Label;
class Surface;

CLASS( TextLine, UIContainer )

	TextLine( const std::string& class_name = "" );

	void Create() override;
	void Destroy() override;

	void SetText( const std::string& text );

	const std::string& GetText() const;
	const std::string* GetTextPtr() const; // be careful

private:
	std::string m_text = "";

	const struct {
		const std::unordered_map< attribute_type_t, attribute_type_t > background = {
			{
				A_ITEM_TEXTURE,
				A_TEXTURE
			},
		};
		const std::unordered_map< attribute_type_t, attribute_type_t > label = {
			{ A_FONT,       A_FONT },
			{ A_TEXT_COLOR, A_TEXT_COLOR },
			{ A_TEXT_ALIGN, A_TEXT_ALIGN },
			{ A_TEXT_LEFT,  A_LEFT },
		};
		const std::unordered_map< attribute_type_t, attribute_type_t > icon = {
			{ A_ITEM_ICON_WIDTH,   A_WIDTH },
			{ A_ITEM_ICON_TEXTURE, A_TEXTURE },
		};
	} m_forwarded_style_attributes;

	Surface* m_background = nullptr;
	Label* m_label = nullptr;
	Surface* m_icon = nullptr;

};

}
}
