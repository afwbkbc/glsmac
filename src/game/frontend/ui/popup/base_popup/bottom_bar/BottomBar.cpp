#include "BottomBar.h"

#include "game/frontend/ui/popup/base_popup/BasePopup.h"
#include "BuildPreview.h"
#include "BuildQueue.h"
#include "BaseTitle.h"
#include "Population.h"
#include "UnitsList.h"
#include "SupportedUnits.h"
#include "game/frontend/base/Base.h"
#include "game/frontend/tile/Tile.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {
namespace bottom_bar {

BottomBar::BottomBar( Game* game, BasePopup* popup )
	: BottomBarBase( game )
	, m_popup( popup ) {
	//
}

void BottomBar::Create() {
	BottomBarBase::Create();

	// sections

	NEW( m_sections.build_preview, BuildPreview, m_game );
	AddChild( m_sections.build_preview );

	NEW( m_sections.build_queue, BuildQueue, m_game );
	AddChild( m_sections.build_queue );

	NEW( m_sections.base_title, BaseTitle, m_game, m_popup );
	AddChild( m_sections.base_title );

	NEW( m_sections.population, Population, m_game );
	AddChild( m_sections.population );

	NEW( m_sections.units_list, UnitsList, m_game, m_popup );
	AddChild( m_sections.units_list );

	NEW( m_sections.supported_units, SupportedUnits, m_game );
	AddChild( m_sections.supported_units );

	m_sections.units_list->ListObjects( m_popup->GetBase()->GetTile()->GetOrderedObjects(), 0 );

}

void BottomBar::Destroy() {

	RemoveChild( m_sections.build_preview );
	RemoveChild( m_sections.build_queue );
	RemoveChild( m_sections.base_title );
	RemoveChild( m_sections.population );
	RemoveChild( m_sections.units_list );
	RemoveChild( m_sections.supported_units );

	BottomBarBase::Destroy();
}

}
}
}
}
}
}
