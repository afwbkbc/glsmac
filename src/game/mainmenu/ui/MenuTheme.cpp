#include "MenuTheme.h"

#include "style/SlidingMenuBlock.h"
#include "style/SlidingMenuButton.h"
#include "style/SlidingMenuButtonUpperTop.h"
#include "style/SlidingMenuButtonLowerTop.h"
#include "style/SlidingMenuButtonLeft.h"
#include "style/SlidingMenuButtonRight.h"
#include "style/SlidingMenuButtonBottom.h"
#include "style/SlidingMenuButtonBody.h"
#include "style/SlidingMenuButtonText.h"
#include "style/SlidingMenuTitleText.h"

namespace game {
namespace mainmenu {

MenuTheme::MenuTheme() : ui::theme::Theme() {
	
	AddStyle( new style::SlidingMenuBlock );
	AddStyle( new style::SlidingMenuButton );
	AddStyle( new style::SlidingMenuButtonUpperTop );
	AddStyle( new style::SlidingMenuButtonLowerTop );
	AddStyle( new style::SlidingMenuButtonLeft );
	AddStyle( new style::SlidingMenuButtonRight );
	AddStyle( new style::SlidingMenuButtonBottom );
	AddStyle( new style::SlidingMenuButtonBody );
	AddStyle( new style::SlidingMenuButtonText );
	AddStyle( new style::SlidingMenuTitleText );
	
}

}
}
