#pragma once

#include <vector>

#include "game/frontend/ui/bottom_bar/objects_list/ObjectsListBase.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {

class BasePopup;

namespace bottom_bar {

class ObjectPreview;

CLASS( UnitsList, ObjectsListBase )

	UnitsList( Game* game, BasePopup* popup );

protected:
	bool OnObjectMouseOver( TileObject* const object ) override;
	bool OnObjectMouseOut( TileObject* const object ) override;
	bool OnObjectClick( TileObject* const object ) override;

private:
	BasePopup* m_popup;
};

}
}
}
}
}
}
