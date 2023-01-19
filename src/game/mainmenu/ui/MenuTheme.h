#pragma once

#include "ui/theme/Theme.h"

#include "style/Common.h"
#include "style/SlidingMenu.h"
#include "style/PopupMenu.h"

namespace game {
namespace mainmenu {

CHILD_CLASS( MenuTheme, ui::theme::Theme )
	MenuTheme();

protected:
	
	struct {
		style::Common common;
		style::SlidingMenu sliding_menu;
		style::PopupMenu popup_menu;
	} m_styles;
};

}
}
