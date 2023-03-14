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
	const size_t GetBorderSize() const;
	
protected:
	
	size_t m_border_size = 1;
	
	virtual void ApplyStyle();
	
	std::unordered_map< Style::attribute_type_t, Surface* > m_surfaces = {
		{ Style::A_TEXTURE_BACK, nullptr },
		{ Style::A_TEXTURE_LEFT, nullptr },
		{ Style::A_TEXTURE_TOP, nullptr },
		{ Style::A_TEXTURE_RIGHT, nullptr },
		{ Style::A_TEXTURE_BOTTOM, nullptr },
		{ Style::A_TEXTURE_LEFTTOP, nullptr },
		{ Style::A_TEXTURE_RIGHTTOP, nullptr },
		{ Style::A_TEXTURE_RIGHTBOTTOM, nullptr },
		{ Style::A_TEXTURE_LEFTBOTTOM, nullptr },
	};
};

}
}
