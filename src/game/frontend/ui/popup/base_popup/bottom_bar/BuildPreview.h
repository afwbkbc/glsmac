#pragma once

#include "game/frontend/ui/bottom_bar/BBSection.h"

namespace game {
namespace frontend {

class TileObject;

namespace ui {
namespace popup {
namespace base_popup {
namespace bottom_bar {

CLASS( BuildPreview, BBSection )

	BuildPreview( Game* game );

	void Create() override;
	void Destroy() override;

};

}
}
}
}
}
}
