#pragma once

#include "ui/theme/Theme.h"

#include "style/Common.h"
#include "style/SlidingMenu.h"

namespace task {
namespace mainmenu {

CLASS( Theme, ::ui::theme::Theme )
	Theme();

protected:

	struct {
		style::Common mainmenu_common;
		style::SlidingMenu sliding_menu;
	} m_styles;
};

}
}
