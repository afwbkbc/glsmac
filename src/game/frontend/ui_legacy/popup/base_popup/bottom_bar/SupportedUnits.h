#pragma once

#include "game/frontend/ui_legacy/bottom_bar/BBSection.h"

namespace game {
namespace frontend {

namespace ui_legacy {
namespace popup {
namespace base_popup {
namespace bottom_bar {

CLASS( SupportedUnits, BBSection )

	SupportedUnits( Game* game );

	void Create() override;
	void Destroy() override;

private:

};

}
}
}
}
}
}
