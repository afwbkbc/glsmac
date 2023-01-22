#pragma once

#include "UIContainer.h"

#include <unordered_map>

#include "types/Color.h"
#include "types/Texture.h"

#include "Surface.h"

namespace ui {
namespace object {

CLASS( Panel, UIContainer )
	Panel( const string& class_name = "" );

	virtual void Create();
	virtual void Destroy();
	
protected:
	virtual void ApplyStyle();
	
	Surface* m_background = nullptr;
	unordered_map<Style::attribute_type_t,Surface* > m_surfaces = {
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
