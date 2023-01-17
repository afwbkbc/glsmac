#include "MenuTheme.h"

#include "style/MainMenuBackground.h"

#include "style/SlidingMenuBlock.h"
#include "style/SlidingMenuItem.h"
#include "style/SlidingMenuItemUpperTop.h"
#include "style/SlidingMenuItemLowerTop.h"
#include "style/SlidingMenuItemLeft.h"
#include "style/SlidingMenuItemRight.h"
#include "style/SlidingMenuItemBottom.h"
#include "style/SlidingMenuItemBody.h"
#include "style/SlidingMenuButtonText.h"
#include "style/SlidingMenuTitleText.h"

#include "style/PopupMenuFrame.h"
#include "style/PopupMenuButton.h"

namespace game {
namespace mainmenu {

using namespace style;

MenuTheme::MenuTheme() : ui::theme::Theme() {
#define ADDSTYLE( _style ) \
	{ \
		NEWV( style, _style ); \
		AddStyle( style ); \
	}
	
	ADDSTYLE( MainMenuBackground );
	
	ADDSTYLE( SlidingMenuBlock );
	ADDSTYLE( SlidingMenuItem );
	ADDSTYLE( SlidingMenuItemUpperTop );
	ADDSTYLE( SlidingMenuItemLowerTop );
	ADDSTYLE( SlidingMenuItemLeft );
	ADDSTYLE( SlidingMenuItemRight );
	ADDSTYLE( SlidingMenuItemBottom );
	ADDSTYLE( SlidingMenuItemBody );
	ADDSTYLE( SlidingMenuButtonText );
	ADDSTYLE( SlidingMenuTitleText );
	
	ADDSTYLE( PopupMenuFrame );
	ADDSTYLE( PopupMenuButton );
	
#undef ADDSTYLE
	
}

}
}
