#include "SaveMap.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

SaveMap::SaveMap( World* world )
	: FilePopup( world, "SAVE MAP", FM_WRITE, "maps", ".gsm", "untitled" )
{
	
}

void SaveMap::OnFileSelect( const std::string& path ) {
	Log( "Selected file: " + path );
}

}
}
}
}
