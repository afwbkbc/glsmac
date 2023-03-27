#include "SaveMap.h"
#include "game/world/World.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

SaveMap::SaveMap( World* world )
	: FilePopup( world, "SAVE MAP", FM_WRITE, "maps", ".gsm", "untitled" )
{
	
}

void SaveMap::OnFileSelect( const std::string& path ) {
	m_world->SaveMap( path );
	Close();
}

}
}
}
}
