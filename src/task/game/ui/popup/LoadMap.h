#pragma once

#include "FilePopup.h"

namespace task {
namespace game {
namespace ui {
namespace popup {

CLASS( LoadMap, FilePopup )
	
	LoadMap( Game* game );
	
protected:
	void OnFileSelect( const std::string& path );

};

}
}
}
}
