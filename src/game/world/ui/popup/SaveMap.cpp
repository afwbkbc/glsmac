#include "SaveMap.h"
#include "game/world/World.h"

#include "util/FS.h"

#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

SaveMap::SaveMap( World* world )
	: FilePopup(
		world,
		"SAVE MAP",
		FM_WRITE,
		world->GetMap()->GetLastDirectory(),
		map::Map::s_consts.fs.default_map_extension,
		world->GetMap()->GetFileName()
	)
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
