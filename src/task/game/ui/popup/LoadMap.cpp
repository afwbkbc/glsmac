#include "LoadMap.h"

#include "task/game/Game.h"
#include "game/backend/map/Consts.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

LoadMap::LoadMap( Game* game )
	: FilePopup(
	game,
	"LOAD MAP",
	FM_READ,
	game->GetMapLastDirectory(),
	::game::backend::map::s_consts.fs.default_map_extension,
	game->GetMapFilename()
) {

}

void LoadMap::OnFileSelect( const std::string& path ) {
	m_game->LoadMap( path );
}

}
}
}
}
