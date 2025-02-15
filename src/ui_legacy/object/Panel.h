#pragma once

#include <unordered_map>

#include "UIContainer.h"

namespace ui_legacy {
namespace object {

class Surface;

CLASS( Panel, UIContainer )
	Panel( const std::string& class_name = "" );

	virtual void Create() override;
	virtual void Destroy() override;
	virtual void Align() override;

	void SetBorderWidth( const size_t border_width );
	const size_t GetBorderSize() const;

protected:

	size_t m_border_size = 1;

	virtual void ApplyStyle() override;

	std::unordered_map< attribute_type_t, ui_legacy::object::Surface* > m_surfaces = {
		{ A_TEXTURE_BACK,        nullptr },
		{ A_TEXTURE_LEFT,        nullptr },
		{ A_TEXTURE_TOP,         nullptr },
		{ A_TEXTURE_RIGHT,       nullptr },
		{ A_TEXTURE_BOTTOM,      nullptr },
		{ A_TEXTURE_LEFTTOP,     nullptr },
		{ A_TEXTURE_RIGHTTOP,    nullptr },
		{ A_TEXTURE_RIGHTBOTTOM, nullptr },
		{ A_TEXTURE_LEFTBOTTOM,  nullptr },
	};
};

}
}
