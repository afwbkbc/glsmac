#include "SlidingMenuTitleText.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuTitleText::SetStyle() {
	SlidingMenuItemText::SetStyle();
	
	SetColor( A_TEXTCOLOR, Color::FromRGBA( 230, 243 ,233, 255 ) );
};


}
}
}
