#pragma once

#include "ui/theme/Theme.h"

// TODO: refactor?
#include "Style.h"
#include "BottomBar.h"
#include "Popup.h"
#include "BasePopup.h"

namespace game {
namespace frontend {
namespace ui {
namespace style {

CLASS( Theme, ::ui::theme::Theme )
	Theme();

protected:

	struct {
		Style style;
		BottomBar bottom_bar;
		Popup popup;
		BasePopup base_popup;
	} m_styles;
};

}
}
}
}
