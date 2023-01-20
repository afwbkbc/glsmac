#include "MenuTheme.h"

#include "style/Common.h"
#include "style/SlidingMenu.h"
#include "style/PopupMenu.h"

namespace game {
namespace mainmenu {

using namespace style;

MenuTheme::MenuTheme() : ui::theme::Theme() {
	
	AddStyleSheet( &m_styles.common );
	AddStyleSheet( &m_styles.sliding_menu );
	AddStyleSheet( &m_styles.popup_menu );
	
}

}
}
