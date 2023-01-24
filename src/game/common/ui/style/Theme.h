#pragma once

#include "ui/theme/Theme.h"

#include "Common.h"
#include "Popup.h"


namespace game {
namespace style {
	
CLASS( Theme, ui::theme::Theme )
	Theme();

protected:
	
	struct {
		Common common;
		Popup popup_menu;
	} m_styles;
};

}
}
