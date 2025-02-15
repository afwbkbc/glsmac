#include "BasePopup.h"

#include "game/frontend/Game.h"
#include "game/frontend/base/Base.h"
#include "game/frontend/base/BaseManager.h"
#include "game/frontend/ui_legacy/popup/base_popup/bottom_bar/BottomBar.h"
#include "engine/Engine.h"
#include "ui_legacy/UI.h"
#include "Governor.h"
#include "Nutrients.h"
#include "Commerce.h"
#include "GlobalInfo.h"
#include "game/frontend/ui_legacy/popup/base_popup/center_area/CenterArea.h"
#include "Yields.h"
#include "Energy.h"
#include "Facilities.h"
#include "Buttons.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {
namespace base_popup {

BasePopup::BasePopup( Game* game, base::Base* base )
	: Popup( game )
	, m_base( base ) {
	SetWidth( 680 );
	SetHeight( 504 );
	m_config.no_inner_border = true;
}

void BasePopup::Create() {
	Popup::Create();

	On(
		::ui_legacy::event::EV_KEY_DOWN, EH( this ) {
			switch ( data->key.code ) {
				case ::ui_legacy::event::K_ENTER: {
					Close();
					break;
				}
				case ::ui_legacy::event::K_LEFT: {
					SelectPrevBase();
					break;
				}
				case ::ui_legacy::event::K_RIGHT: {
					SelectNextBase();
					break;
				}
				default:
					return false;
			}
			return true;
		}
	);

	NEW( m_sections.governor, Governor, this );
	AddChild( m_sections.governor );

	NEW( m_sections.nutrients, Nutrients, this );
	AddChild( m_sections.nutrients );

	NEW( m_sections.commerce, Commerce, this );
	AddChild( m_sections.commerce );

	NEW( m_sections.global_info, GlobalInfo, this );
	AddChild( m_sections.global_info );

	NEW( m_sections.center_area, center_area::CenterArea, this );
	AddChild( m_sections.center_area );

	NEW( m_sections.yields, Yields, this );
	AddChild( m_sections.yields );

	NEW( m_sections.energy, Energy, this );
	AddChild( m_sections.energy );

	NEW( m_sections.facilities, Facilities, this );
	AddChild( m_sections.facilities );

	NEW( m_sections.buttons, Buttons, this );
	AddChild( m_sections.buttons );

	m_sections.center_area->Update( m_base );
}

void BasePopup::Destroy() {

	RemoveChild( m_sections.governor );
	RemoveChild( m_sections.nutrients );
	RemoveChild( m_sections.commerce );
	RemoveChild( m_sections.global_info );
	RemoveChild( m_sections.center_area );
	RemoveChild( m_sections.yields );
	RemoveChild( m_sections.energy );
	RemoveChild( m_sections.facilities );
	RemoveChild( m_sections.buttons );

	Popup::Destroy();
}

Game* BasePopup::GetGame() const {
	return m_game;
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

	m_bottom_bar->Update( m_base );
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
	m_sections.center_area->Update( m_base );
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
