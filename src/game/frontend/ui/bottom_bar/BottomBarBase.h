#pragma once

#include "game/frontend/ui/UI.h"

namespace ui::object {
class Surface;
}

namespace game {
namespace frontend {
class Game;

namespace ui {

CLASS( BottomBarBase, UI )

	BottomBarBase( Game* game );

	void Create() override;
	void Destroy() override;

private:

	struct {
		::ui::object::Surface* left = nullptr;
		::ui::object::Surface* right = nullptr;
		::ui::object::Surface* middle = nullptr;
	} m_frames = {};

	std::vector< ::ui::object::Surface* > m_backgrounds = {};

};

}
}
}
