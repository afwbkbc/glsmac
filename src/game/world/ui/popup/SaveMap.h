#pragma once

#include "FilePopup.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

CLASS( SaveMap, FilePopup )
	
	SaveMap( World* world );
	
protected:
	void OnFileSelect( const std::string& path );

};

}
}
}
}
