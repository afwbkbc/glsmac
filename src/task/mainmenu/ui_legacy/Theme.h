#pragma once

#include "ui_legacy/theme/Theme.h"

#include "style/Common.h"
#include "style/SlidingMenu.h"

namespace task {
namespace mainmenu {

CLASS( Theme, ::ui_legacy::theme::Theme )
	Theme();

protected:

	struct {
		style::Common mainmenu_common;
		style::SlidingMenu sliding_menu;
	} m_styles;
};

}
}
