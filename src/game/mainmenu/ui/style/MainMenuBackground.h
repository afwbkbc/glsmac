#pragma once

#include "ui/theme/Style.h"

namespace game {
namespace mainmenu {

CHILD_CLASS( MainMenuBackground, ui::theme::Style )
protected:
	void SetStyle();

};

}
}