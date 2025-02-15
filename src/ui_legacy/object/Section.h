#pragma once

#include "UIContainer.h"

namespace ui_legacy {
namespace object {

class Panel;
class Label;

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
	const std::unordered_map< attribute_type_t, attribute_type_t > m_forwarded_style_attributes = {
		{ A_BORDER_SIZE,           A_BORDER_SIZE },
		{ A_HEADER_TEXTURE_BACK,   A_HEADER_TEXTURE_BACK },
		{ A_HEADER_TEXTURE_LEFT,   A_HEADER_TEXTURE_LEFT },
		{ A_HEADER_TEXTURE_TOP,    A_HEADER_TEXTURE_TOP },
		{ A_HEADER_TEXTURE_RIGHT,  A_HEADER_TEXTURE_RIGHT },
		{ A_HEADER_TEXTURE_BOTTOM, A_HEADER_TEXTURE_BOTTOM },
		{ A_HEADER_FONT,           A_HEADER_FONT },
		{ A_HEADER_TEXTCOLOR,      A_HEADER_TEXTCOLOR },
		{ A_HEADER_TEXTALIGN,      A_HEADER_TEXTALIGN },
		{ A_TEXTURE_BACK,          A_TEXTURE_BACK },
		{ A_TEXTURE_LEFT,          A_TEXTURE_LEFT },
		{ A_TEXTURE_LEFTTOP,       A_TEXTURE_LEFTTOP },
		{ A_TEXTURE_TOP,           A_TEXTURE_TOP },
		{ A_TEXTURE_RIGHTTOP,      A_TEXTURE_RIGHTTOP },
		{ A_TEXTURE_RIGHT,         A_TEXTURE_RIGHT },
		{ A_TEXTURE_RIGHTBOTTOM,   A_TEXTURE_RIGHTBOTTOM },
		{ A_TEXTURE_BOTTOM,        A_TEXTURE_BOTTOM },
		{ A_TEXTURE_LEFTBOTTOM,    A_TEXTURE_LEFTBOTTOM },
		{ A_HEADER_HEIGHT,         A_HEADER_HEIGHT },
	};

	const std::unordered_map< attribute_type_t, attribute_type_t > m_forwarded_header_style_attributes = {
		{ A_HEADER_TEXTURE_BACK,   A_TEXTURE_BACK },
		{ A_HEADER_TEXTURE_LEFT,   A_TEXTURE_LEFT },
		{ A_HEADER_TEXTURE_TOP,    A_TEXTURE_TOP },
		{ A_HEADER_TEXTURE_RIGHT,  A_TEXTURE_RIGHT },
		{ A_HEADER_TEXTURE_BOTTOM, A_TEXTURE_BOTTOM },
		{ A_HEADER_FONT,           A_HEADER_FONT },
		{ A_HEADER_TEXTCOLOR,      A_HEADER_TEXTCOLOR },
		{ A_HEADER_TEXTALIGN,      A_HEADER_TEXTALIGN },
		{ A_HEADER_HEIGHT,         A_HEIGHT },
	};

	const std::unordered_map< attribute_type_t, attribute_type_t > m_forwarded_header_label_style_attributes = {
		{ A_HEADER_FONT,      A_FONT },
		{ A_HEADER_TEXTCOLOR, A_TEXT_COLOR },
		{ A_HEADER_TEXTALIGN, A_TEXT_ALIGN },
	};
};

}
}
