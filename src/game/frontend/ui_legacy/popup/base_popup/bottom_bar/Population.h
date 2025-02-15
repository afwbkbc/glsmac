#pragma once

#include <vector>

#include "game/frontend/ui_legacy/bottom_bar/BBSection.h"

namespace ui_legacy::object {
class Surface;
}

namespace game {
namespace frontend {

namespace base {
class Base;
}

namespace ui_legacy {
namespace popup {
namespace base_popup {
namespace bottom_bar {

CLASS( Population, BBSection )

	Population( Game* game );

	void Create() override;
	void Destroy() override;

	void Update( base::Base* base );

private:
	struct pop_info_t {
		size_t pop_id;
		::ui_legacy::object::Surface* icon;
	};
	std::vector< pop_info_t > m_pops = {};

	void HideIcons();

};

}
}
}
}
}
}
