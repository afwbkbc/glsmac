#include "EditMapMenu.h"

#include "engine/Engine.h"

#include "../../popup/Save.h"

namespace game {
namespace world {
namespace ui {
namespace menu {

EditMapMenu::EditMapMenu( World* world )
	: Menu( world )
{
	auto* ui = g_engine->GetUI();
	AddItem( "Save Map...", MH( this, ui ) {
		NEWV( popup, popup::Save, m_world );
			popup->SetWidth( 600 );
			popup->SetHeight( 500 );
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
