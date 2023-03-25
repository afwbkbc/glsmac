#pragma once

#include "OkCancelPopup.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

CLASS( SaveMap, OkCancelPopup )
	
	SaveMap( World* world );
	
	void Create();
	void Destroy();

protected:
	void OnOk();
	
};

}
}
}
}
