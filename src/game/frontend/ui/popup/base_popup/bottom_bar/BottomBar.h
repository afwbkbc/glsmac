#pragma once

#include "game/frontend/ui/bottom_bar/BottomBarBase.h"

#include "game/backend/turn/Types.h"

#include "types/Vec2.h"

namespace game {
namespace frontend {
class Game;

namespace ui {
namespace popup {
namespace base_popup {

class BasePopup;

namespace bottom_bar {

class BuildPreview;
class BuildQueue;
class BaseTitle;
class Population;
class UnitsList;
class SupportedUnits;

CLASS( BottomBar, BottomBarBase )

	BottomBar( Game* game, BasePopup* popup );

	void Create() override;
	void Destroy() override;

private:

	BasePopup* m_popup = nullptr;

	struct {
		BuildPreview* build_preview = nullptr;
		BuildQueue* build_queue = nullptr;
		BaseTitle* base_title = nullptr;
		Population* population = nullptr;
		UnitsList* units_list = nullptr;
		SupportedUnits* supported_units = nullptr;
	} m_sections = {};

};

}
}
}
}
}
}
