#pragma once

#include "ui_legacy/theme/Theme.h"

namespace task {
namespace style {

class Common;
class Popup;
class Scrollbars;

CLASS( Theme, ::ui_legacy::theme::Theme )
	Theme();
	~Theme();

protected:

	struct {
		Common* common;
		Popup* popup;
		Scrollbars* scrollbars;
	} m_styles;
};

}
}
