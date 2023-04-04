#include "SaveMap.h"
#include "task/game/Game.h"

#include "util/FS.h"

#include "engine/Engine.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

SaveMap::SaveMap( Game* game )
	: FilePopup(
		game,
		"SAVE MAP",
		FM_WRITE,
		game->GetMap()->GetLastDirectory(),
		map::Map::s_consts.fs.default_map_extension,
		game->GetMap()->GetFileName()
	)
{
	
}

void SaveMap::OnFileSelect( const std::string& path ) {
	m_game->SaveMap( path );
	Close();
}

}
}
}
}
