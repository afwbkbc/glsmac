#include "PopupMenuHeaderLabel.h"

#include "engine/Engine.h"

namespace game {
namespace mainmenu {
namespace style {

void PopupMenuHeaderLabel::SetStyle() {
	SetObject( A_FONT, g_engine->GetFontLoader()->LoadFont( "arialnb.ttf", 18 ) );
	SetColor( A_TEXTCOLOR, Color::FromRGB( 109, 126, 178 ) );

}

}
}
}
