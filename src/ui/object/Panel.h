#pragma once

#include <unordered_map>

#include "UIContainer.h"

#include "types/Color.h"
#include "types/Texture.h"

#include "Surface.h"

namespace ui {
namespace object {

CLASS( Panel, UIContainer )
	Panel( const std::string& class_name = "" );

	virtual void Create();
	virtual void Destroy();
	virtual void Align();
	
	void SetBorderWidth( const size_t border_width );
	
protected:
	
	size_t m_border_width = 1;
	
	virtual void ApplyStyle();
	
	Surface* m_background = nullptr;
	std::unordered_map<Style::attribute_type_t,Surface* > m_surfaces = {
		{ Style::A_TEXTURE_BACK, nullptr },
		{ Style::A_TEXTURE_BORDER_LEFT, nullptr },
		{ Style::A_TEXTURE_BORDER_TOP, nullptr },
		{ Style::A_TEXTURE_BORDER_RIGHT, nullptr },
		{ Style::A_TEXTURE_BORDER_BOTTOM, nullptr },
		{ Style::A_TEXTURE_BORDER_CORNER_LT, nullptr },
		{ Style::A_TEXTURE_BORDER_CORNER_RT, nullptr },
		{ Style::A_TEXTURE_BORDER_CORNER_RB, nullptr },
		{ Style::A_TEXTURE_BORDER_CORNER_LB, nullptr },
	};
};

}
}
