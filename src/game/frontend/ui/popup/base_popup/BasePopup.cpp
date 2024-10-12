#include "BasePopup.h"

#include "game/frontend/Game.h"
#include "game/frontend/base/Base.h"
#include "game/frontend/base/BaseManager.h"
#include "game/frontend/ui/popup/base_popup/bottom_bar/BottomBar.h"
#include "engine/Engine.h"
#include "ui/UI.h"

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
			switch ( data->key.code ) {
				case ::ui::event::K_ENTER: {
					Close();
					break;
				}
				case ::ui::event::K_LEFT: {
					SelectPrevBase();
					break;
				}
				case ::ui::event::K_RIGHT: {
					SelectNextBase();
					break;
				}
				default:
					return false;
			}
			return true;
		}
	);

}

void BasePopup::Destroy() {

	Popup::Destroy();
}

base::Base* BasePopup::GetBase() const {
	return m_base;
}

void BasePopup::OnOpen() {

	// show base-specific bottom bar
	m_game->HideBottomBar();
	NEW( m_bottom_bar, bottom_bar::BottomBar, m_game, this );
	g_engine->GetUI()->AddObject( m_bottom_bar );
	AddEventsTarget( m_bottom_bar );

	Update( m_base );

}

void BasePopup::OnClose() {

	// restore main bottom bar
	RemoveEventsTarget( m_bottom_bar );
	g_engine->GetUI()->RemoveObject( m_bottom_bar );
	m_game->ShowBottomBar();

	m_game->OnBasePopupClose();
}

void BasePopup::Update( base::Base* base ) {
	if ( m_base != base ) {
		m_base = base;
		m_game->SelectAnyUnitAtTile( m_base->GetTile() );
	}
	m_bottom_bar->Update( m_base );
}

void BasePopup::SelectNextBase() {
	auto* base = m_game->GetBM()->GetBaseAfter( m_base );
	Update( base );
}

void BasePopup::SelectPrevBase() {
	auto* base = m_game->GetBM()->GetBaseBefore( m_base );
	Update( base );
}

}
}
}
}
}
