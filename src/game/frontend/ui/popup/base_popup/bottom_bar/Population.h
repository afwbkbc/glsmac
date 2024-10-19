#pragma once

#include <vector>

#include "game/frontend/ui/bottom_bar/BBSection.h"

namespace ui::object {
class Surface;
}

namespace game {
namespace frontend {

namespace base {
class Base;
}

namespace ui {
namespace popup {
namespace base_popup {
namespace bottom_bar {

CLASS( Population, BBSection )

	Population( Game* game );

	void Create() override;
	void Destroy() override;

	void Update( base::Base* base );

private:
	std::vector< ::ui::object::Surface* > m_icons = {};

	void HideIcons();

};

}
}
}
}
}
}
