#include "SaveMap.h"
#include "game/world/World.h"

#include "Confirmation.h"
#include "util/FS.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

SaveMap::SaveMap( World* world )
	: FilePopup( world, "SAVE MAP", FM_WRITE, "maps", ".gsm", "untitled" )
{
	
}

void SaveMap::OnFileSelect( const std::string& path ) {

	const auto f_save_and_close = [ this, path ] () -> void {
		m_world->SaveMap( path );
		Close();
	};
	
	if ( util::FS::FileExists( path ) ) {
		NEWV( popup, Confirmation, m_world );
			popup->SetText( "File " + util::FS::GetBaseName( path ) + " already exists! Overwrite?" );
			popup->On( UIEvent::EV_CONFIRM, EH( f_save_and_close ) {
				f_save_and_close();
				return true;
			});
		popup->Open();
	}
	else {
		f_save_and_close();
	}
}

}
}
}
}
