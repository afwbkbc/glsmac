#pragma once

#include "../UI.h"

#include "ui/object/Surface.h"

namespace game {
namespace world {
namespace ui {
	
CLASS( BottomBar, UI )
	
	BottomBar( const World* world ) : UI( world, "MapBottomBar" ) {}

	void Create();
	void Destroy();
	
protected:
	Surface* m_frame = nullptr;

};
	
}
}
}
