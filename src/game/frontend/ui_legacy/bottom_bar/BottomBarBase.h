#pragma once

#include "game/frontend/ui_legacy/UI.h"

namespace ui_legacy::object {
class Surface;
}

namespace game {
namespace frontend {
class Game;

namespace ui_legacy {

CLASS( BottomBarBase, UI )

	BottomBarBase( Game* game );

	void Create() override;
	void Destroy() override;

private:

	struct {
		::ui_legacy::object::Surface* left = nullptr;
		::ui_legacy::object::Surface* right = nullptr;
		::ui_legacy::object::Surface* middle = nullptr;
	} m_frames = {};

	std::vector< ::ui_legacy::object::Surface* > m_backgrounds = {};

};

}
}
}
