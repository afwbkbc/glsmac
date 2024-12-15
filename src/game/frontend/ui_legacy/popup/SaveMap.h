#pragma once

#include "FilePopup.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {

CLASS( SaveMap, FilePopup )

	SaveMap( Game* game );

protected:
	void OnFileSelect( const std::string& path ) override;

};

}
}
}
}
