#pragma once

#include "../Popup.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

CLASS( Save, Popup )
	
	Save( World* world ) : Popup( world ) {}
	
	void Create();
	void Destroy();

};

}
}
}
}
