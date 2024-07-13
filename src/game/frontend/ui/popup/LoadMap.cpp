#include "LoadMap.h"

#include "game/frontend/Game.h"
#include "game/backend/map/Consts.h"

namespace game {
namespace frontend {
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
