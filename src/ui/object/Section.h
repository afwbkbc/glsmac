#pragma once

#include "UIContainer.h"

#include "Panel.h"
#include "Label.h"

#include "Surface.h"

namespace ui {
namespace object {

CLASS( Section, UIContainer )

	Section( const std::string& class_name = "" );

	void SetTitleText( const std::string& title_text );

	void AddChild( UIObject* object );
	void RemoveChild( UIObject* object );

	virtual void Create() override;
	virtual void Destroy() override;

protected:
	void ApplyStyle() override;

	std::string m_title_text = "";

	Panel* m_frame = nullptr;
	Panel* m_titlebar = nullptr;
	Label* m_titlelabel = nullptr;
	Panel* m_body = nullptr;

	// TODO: style attribute scopes/inheritance
	const std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > m_forwarded_style_attributes = {
		{ Style::A_BORDER_SIZE,           Style::A_BORDER_SIZE },
		{ Style::A_HEADER_TEXTURE_BACK,   Style::A_HEADER_TEXTURE_BACK },
		{ Style::A_HEADER_TEXTURE_LEFT,   Style::A_HEADER_TEXTURE_LEFT },
		{ Style::A_HEADER_TEXTURE_TOP,    Style::A_HEADER_TEXTURE_TOP },
		{ Style::A_HEADER_TEXTURE_RIGHT,  Style::A_HEADER_TEXTURE_RIGHT },
		{ Style::A_HEADER_TEXTURE_BOTTOM, Style::A_HEADER_TEXTURE_BOTTOM },
		{ Style::A_HEADER_FONT,           Style::A_HEADER_FONT },
		{ Style::A_HEADER_TEXTCOLOR,      Style::A_HEADER_TEXTCOLOR },
		{ Style::A_HEADER_TEXTALIGN,      Style::A_HEADER_TEXTALIGN },
		{ Style::A_TEXTURE_BACK,          Style::A_TEXTURE_BACK },
		{ Style::A_TEXTURE_LEFT,          Style::A_TEXTURE_LEFT },
		{ Style::A_TEXTURE_LEFTTOP,       Style::A_TEXTURE_LEFTTOP },
		{ Style::A_TEXTURE_TOP,           Style::A_TEXTURE_TOP },
		{ Style::A_TEXTURE_RIGHTTOP,      Style::A_TEXTURE_RIGHTTOP },
		{ Style::A_TEXTURE_RIGHT,         Style::A_TEXTURE_RIGHT },
		{ Style::A_TEXTURE_RIGHTBOTTOM,   Style::A_TEXTURE_RIGHTBOTTOM },
		{ Style::A_TEXTURE_BOTTOM,        Style::A_TEXTURE_BOTTOM },
		{ Style::A_TEXTURE_LEFTBOTTOM,    Style::A_TEXTURE_LEFTBOTTOM },
		{ Style::A_HEADER_HEIGHT,         Style::A_HEADER_HEIGHT },
	};

	const std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > m_forwarded_header_style_attributes = {
		{ Style::A_HEADER_TEXTURE_BACK,   Style::A_TEXTURE_BACK },
		{ Style::A_HEADER_TEXTURE_LEFT,   Style::A_TEXTURE_LEFT },
		{ Style::A_HEADER_TEXTURE_TOP,    Style::A_TEXTURE_TOP },
		{ Style::A_HEADER_TEXTURE_RIGHT,  Style::A_TEXTURE_RIGHT },
		{ Style::A_HEADER_TEXTURE_BOTTOM, Style::A_TEXTURE_BOTTOM },
		{ Style::A_HEADER_FONT,           Style::A_HEADER_FONT },
		{ Style::A_HEADER_TEXTCOLOR,      Style::A_HEADER_TEXTCOLOR },
		{ Style::A_HEADER_TEXTALIGN,      Style::A_HEADER_TEXTALIGN },
		{ Style::A_HEADER_HEIGHT,         Style::A_HEIGHT },
	};

	const std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > m_forwarded_header_label_style_attributes = {
		{ Style::A_HEADER_FONT,      Style::A_FONT },
		{ Style::A_HEADER_TEXTCOLOR, Style::A_TEXT_COLOR },
		{ Style::A_HEADER_TEXTALIGN, Style::A_TEXT_ALIGN },
	};
};

}
}
