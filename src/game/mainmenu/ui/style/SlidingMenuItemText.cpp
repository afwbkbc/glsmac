#include "SlidingMenuItemText.h"

#include "engine/Engine.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuItemText::SetStyle() {
	SetObject( A_FONT, g_engine->GetFontLoader()->LoadFont( "arialnb.ttf", 20 ) );
};


}
}
}
