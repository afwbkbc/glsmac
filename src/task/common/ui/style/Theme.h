#pragma once

#include "ui/theme/Theme.h"

#include "Common.h"
#include "Popup.h"
#include "Scrollbars.h"

namespace task {
namespace style {

CLASS( Theme, ui::theme::Theme )
	Theme();

protected:

	struct {
		Common common;
		Popup popup_menu;
		Scrollbars scrollbars;
	} m_styles;
};

}
}
