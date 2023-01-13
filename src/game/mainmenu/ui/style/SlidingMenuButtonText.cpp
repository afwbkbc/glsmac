#include "SlidingMenuButtonText.h"

#include "engine/Engine.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuButtonText::SetStyle() {
	SlidingMenuItemText::SetStyle();

	if ( m_modifier == M_HOVER ) {
		SetColor( A_COLOR, Color::FromRGBA( 193, 240 ,204, 255 ) );
	}
	else {
		SetColor( A_COLOR, Color::FromRGBA( 94, 162 ,137, 255 ) );
	}
}

}
}
}
