#pragma once

#include "ui/theme/Theme.h"

#include "style/Common.h"
#include "style/SlidingMenu.h"
#include "style/PopupMenu.h"

namespace game {
namespace mainmenu {

CLASS( Theme, ui::theme::Theme )
	Theme();

protected:
	
	struct {
		style::Common common;
		style::SlidingMenu sliding_menu;
		style::PopupMenu popup_menu;
	} m_styles;
};

}
}
