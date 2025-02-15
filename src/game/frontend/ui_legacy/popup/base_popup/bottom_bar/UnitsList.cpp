#include "UnitsList.h"

#include "game/frontend/TileObject.h"
#include "game/frontend/ui_legacy/popup/base_popup/BasePopup.h"
#include "game/frontend/Game.h"
#include "game/frontend/unit/UnitManager.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {
namespace base_popup {
namespace bottom_bar {

UnitsList::UnitsList( Game* game, BasePopup* popup )
	: ObjectsListBase( game )
	, m_popup( popup ) {}

bool UnitsList::OnObjectMouseOver( TileObject* const object ) {
	return true;
}

bool UnitsList::OnObjectMouseOut( TileObject* const object ) {
	return true;
}

bool UnitsList::OnObjectClick( TileObject* const object ) {
	if ( object->GetType() == TileObject::TOT_UNIT ) {
		m_game->SetBasePopupSelectedUnit( (unit::Unit*)object );
		m_popup->Close();
	}
	return true;
}

}
}
}
}
}
}
