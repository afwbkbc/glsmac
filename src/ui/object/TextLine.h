#pragma once

#include "UIContainer.h"

#include "Label.h"
#include "Surface.h"

namespace ui {
namespace object {

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
		const std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > background = {
			{ Style::A_ITEM_TEXTURE, Style::A_TEXTURE },
		};
		const std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > label = {
			{ Style::A_FONT, Style::A_FONT },
			{ Style::A_TEXT_COLOR, Style::A_TEXT_COLOR },
			{ Style::A_TEXT_ALIGN, Style::A_TEXT_ALIGN },
			{ Style::A_TEXT_LEFT, Style::A_LEFT },
		};
		const std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > icon = {
			{ Style::A_ITEM_ICON_WIDTH, Style::A_WIDTH },
			{ Style::A_ITEM_ICON_TEXTURE, Style::A_TEXTURE },
		};
	} m_forwarded_style_attributes;
	
	Surface* m_background = nullptr;
	Label* m_label = nullptr;
	Surface* m_icon = nullptr;
	
};

}
}
