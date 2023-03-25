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
	
}

void SaveMap::Destroy() {
	
	OkCancelPopup::Destroy();
}

void SaveMap::OnOk() {
	Log( "OK CLICKED" );
}

}
}
}
}
