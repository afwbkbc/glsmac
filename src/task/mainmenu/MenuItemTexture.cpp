#include "MenuItemTexture.h"
#include "types/Color.h"

namespace task {
namespace mainmenu {

MenuItemTexture::MenuItemTexture() : Texture( "MainMenu::MenuItemTexture", 360, 72 ) {
	
	const types::Color
		background = { 0.098, 0.18, 0.106, 1.0 },
		border = { 0.298, 0.298, 0.267, 1.0 }
	;
	
	Rectangle( 19, 30, 308, 66, background);
	Rectangle( 14, 18, 324, 26, border );
	
}

}
}
