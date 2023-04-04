#include "LoadMap.h"
#include "task/game/Game.h"

#include "util/FS.h"

#include "engine/Engine.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

LoadMap::LoadMap( Game* game )
	: FilePopup(
		game,
		"LOAD MAP",
		FM_READ,
		game->GetMap()->GetLastDirectory(),
		map::Map::s_consts.fs.default_map_extension,
		game->GetMap()->GetFileName()
	)
{
	
}

void LoadMap::OnFileSelect( const std::string& path ) {
	m_game->LoadMap( path );
	Close();
}

}
}
}
}
