#include "BottomBar.h"

namespace game {
namespace world {
namespace ui {

void BottomBar::Create() {
	UI::Create();
	
	NEW( m_frame, Surface, "MapBottomBarFrame" );
	
	AddChild( m_frame );
}

void BottomBar::Destroy() {
	RemoveChild( m_frame );
	
	UI::Destroy();
}

/*	NEW( m_body, Panel );
		m_body->SetAlign( UIObject::ALIGN_TOP );
		if ( HasFlag( PF_HAS_OK ) || HasFlag( PF_HAS_CANCEL ) ) {
			m_body->SetBottom( 40 );
		}
	m_frame->AddChild( m_body );*/
	


}
}
}
