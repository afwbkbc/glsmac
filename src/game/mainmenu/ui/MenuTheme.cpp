#include "MenuTheme.h"

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

namespace game {
namespace mainmenu {

MenuTheme::MenuTheme() : ui::theme::Theme() {
	
	AddStyle( new style::SlidingMenuBlock );
	AddStyle( new style::SlidingMenuItem );
	AddStyle( new style::SlidingMenuItemUpperTop );
	AddStyle( new style::SlidingMenuItemLowerTop );
	AddStyle( new style::SlidingMenuItemLeft );
	AddStyle( new style::SlidingMenuItemRight );
	AddStyle( new style::SlidingMenuItemBottom );
	AddStyle( new style::SlidingMenuItemBody );
	AddStyle( new style::SlidingMenuButtonText );
	AddStyle( new style::SlidingMenuTitleText );
	
}

}
}
