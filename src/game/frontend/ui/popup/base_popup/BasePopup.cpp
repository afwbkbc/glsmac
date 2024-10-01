#include "BasePopup.h"

#include "game/frontend/Game.h"
#include "game/frontend/base/Base.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {

BasePopup::BasePopup( Game* game, base::Base* base )
	: Popup( game )
	, m_base( base ) {
	SetWidth( 680 );
	SetHeight( 480 );
}

void BasePopup::Create() {
	Popup::Create();

	On(
		::ui::event::EV_KEY_DOWN, EH( this ) {
			if ( data->key.code == ::ui::event::K_ENTER ) {
				Close();
				return true;
			}
			return false;
		}
	);
}

void BasePopup::Destroy() {

	Popup::Destroy();
}

base::Base* BasePopup::GetBase() const {
	return m_base;
}

void BasePopup::OnClose() {
	m_game->OnBasePopupClose();
}

}
}
}
}
}
