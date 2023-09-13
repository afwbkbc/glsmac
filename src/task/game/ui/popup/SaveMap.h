#pragma once

#include "FilePopup.h"

namespace task {
namespace game {
namespace ui {
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
