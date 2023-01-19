#include "PopupMenuButtonOkCancel.h"

#include "engine/Engine.h"

namespace game {
namespace mainmenu {
namespace style {

void PopupMenuButtonOkCancel::SetStyle() {
	PopupMenuButton::SetStyle();
	
	Set( A_WIDTH, 234 );
	Set( A_HEIGHT, 20 );

	SetObject( A_FONT, g_engine->GetFontLoader()->LoadFont( "arialnb.ttf", 18 ) );

}

}
}
}
