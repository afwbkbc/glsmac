#include "TilePreview.h"

namespace game {
namespace world {
namespace ui {

void TilePreview::Create() {
	UI::Create();
	
	NEW( m_outer, Section, "MapBottomBarSectionOuter" );
	
	AddChild( m_outer );
	
	NEW( m_inner, Section, "MapBottomBarSectionInner" );
	m_outer->AddChild( m_inner );
}

void TilePreview::Destroy() {
	m_outer->RemoveChild( m_inner );
	RemoveChild( m_outer );
	
	UI::Destroy();
}


}
}
}
