#pragma once

#include "UIContainer.h"

#include "types/Color.h"
#include "types/Texture.h"

#include "Surface.h"

namespace ui {
namespace object {

CHILD_CLASS( Panel, UIContainer )
	Panel( const string& class_name = "" );

	virtual void Create();
	virtual void Destroy();
	
protected:
	virtual void ApplyStyle();
	
	Surface* m_background = nullptr;
	unordered_map<Style::attribute_type_t,Surface* > m_borders = {
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
