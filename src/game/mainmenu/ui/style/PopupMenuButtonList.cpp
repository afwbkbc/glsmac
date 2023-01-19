#include "PopupMenuButtonList.h"

#include "engine/Engine.h"

using namespace ui;
namespace ui {
using namespace object;
}

namespace game {
namespace mainmenu {
namespace style {

void PopupMenuButtonList::SetStyle() {
	PopupMenuButton::SetStyle();
	
	Set( A_LEFT, 3 );
	Set( A_RIGHT, 3 );
	//Set( A_TOP, 3 );
	Set( A_BOTTOM, 3 );

	SetObject( A_FONT, g_engine->GetFontLoader()->LoadFont( "arialnb.ttf", 16 ) );
	Set( A_TEXTALIGN, UIObject::ALIGN_LEFT );

}

}
}
}
