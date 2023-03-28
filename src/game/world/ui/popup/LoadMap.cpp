#include "LoadMap.h"
#include "game/world/World.h"

#include "util/FS.h"

#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

LoadMap::LoadMap( World* world )
	: FilePopup(
		world,
		"LOAD MAP",
		FM_READ,
		world->GetMap()->GetLastDirectory(),
		map::Map::s_consts.fs.default_map_extension,
		world->GetMap()->GetFileName()
	)
{
	
}

void LoadMap::OnFileSelect( const std::string& path ) {
	m_world->LoadMap( path );
	Close();
}

}
}
}
}
