#pragma once

#include "ui/theme/Theme.h"

#include "Style.h"

namespace game {
namespace world {
namespace ui {

CLASS( Theme, ::ui::theme::Theme )
	Theme();

protected:
	
	struct {
		Style style;
	} m_styles;
};

}
}
}
