#pragma once

#include "ui_legacy/theme/Theme.h"

// TODO: refactor?
#include "Style.h"
#include "BottomBar.h"
#include "Popup.h"
#include "BasePopup.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace style {

CLASS( Theme, ::ui_legacy::theme::Theme )
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
