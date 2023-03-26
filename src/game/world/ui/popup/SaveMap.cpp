#include "SaveMap.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

SaveMap::SaveMap( World* world )
	: OkCancelPopup( world )
{
	SetWidth( 400 );
	SetHeight( 480 );
	SetTitleText( "SAVE MAP" );
}

void SaveMap::Create() {
	OkCancelPopup::Create();
	
	NEW( m_file_browser, ::ui::object::FileBrowser );
		// TODO: determine position from style
		m_file_browser->SetTop( 36 );
		m_file_browser->SetLeft( 6 );
		m_file_browser->SetBottom( 66 );
		m_file_browser->SetRight( 6 );
	AddChild( m_file_browser );
}

void SaveMap::Destroy() {
	RemoveChild( m_file_browser );
	
	OkCancelPopup::Destroy();
}

void SaveMap::OnOk() {
	Log( "OK CLICKED" );
}

}
}
}
}
