#include "SaveMap.h"

#include "game/frontend/Game.h"
#include "game/backend/map/Consts.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {

SaveMap::SaveMap( Game* game )
	: FilePopup(
	game,
	"SAVE MAP",
	FM_WRITE,
	game->GetMapLastDirectory(),
	::game::backend::map::s_consts.fs.default_map_extension,
	game->GetMapFilename()
) {

}

void SaveMap::OnFileSelect( const std::string& path ) {
	m_game->SaveMap( path );
}

}
}
}
}
