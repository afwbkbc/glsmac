#pragma once

#include "game/frontend/ui_legacy/Section.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {
namespace base_popup {

class BasePopup;

CLASS( BPSection, Section )

	BPSection( BasePopup* popup, const std::string class_name = "" );

protected:
	BasePopup* const m_popup = nullptr;

private:

};

}
}
}
}
}
