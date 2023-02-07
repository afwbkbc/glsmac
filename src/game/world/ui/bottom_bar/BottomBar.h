#pragma once

#include "../UI.h"

#include "ui/object/Surface.h"
#include "ui/object/Section.h"
#include "ui/object/Button.h"

namespace game {
namespace world {
namespace ui {
	
CLASS( BottomBar, UI )
	
	BottomBar( const World* world ) : UI( world, "MapBottomBar" ) {}

	void Create();
	void Destroy();
	
protected:
	struct {
		Surface* left = nullptr;
		Surface* right = nullptr;
		Surface* middle = nullptr;
	} m_frames;
	
	vector< Surface* > m_backgrounds;
	
	struct {
		Button* menu = nullptr;
		Button* commlink = nullptr;
	} m_buttons;
	
	Section* m_unit_preview = nullptr;
};
	
}
}
}
