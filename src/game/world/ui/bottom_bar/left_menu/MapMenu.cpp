#include "MapMenu.h"

#include "engine/Engine.h"
#include "ui/object/Popup.h"

namespace game {
namespace world {
namespace ui {
namespace menu {

MapMenu::MapMenu( World* world )
	: Menu( world )
{
	auto* ui = g_engine->GetUI();
	AddItem( "Save Map...", MH( ui ) {
		NEWV( popup, Popup );
			popup->SetWidth( 600 );
			popup->SetBottom( 260 );
			popup->SetTop( 80 );
			ui->OpenPopup( popup );
		return true;
	});
	AddItem( "Load Map...", MH( ui ) {
		return true;
	});
}

}
}
}
}
