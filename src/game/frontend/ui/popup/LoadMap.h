#pragma once

#include "FilePopup.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {

CLASS( LoadMap, FilePopup )

	LoadMap( Game* game );

protected:
	void OnFileSelect( const std::string& path ) override;

};

}
}
}
}
