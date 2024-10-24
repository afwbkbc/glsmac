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
	struct pop_info_t {
		size_t pop_id;
		::ui::object::Surface* icon;
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
