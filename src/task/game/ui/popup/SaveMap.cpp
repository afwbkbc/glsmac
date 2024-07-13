#include "SaveMap.h"

#include "task/game/Game.h"
#include "game/backend/map/Consts.h"

namespace task {
namespace game {
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
