#pragma once

#include "MiddleAreaPage.h"

namespace game {
namespace world {
	
namespace ui {
	
CLASS( Toolbar, MiddleAreaPage )
	
	Toolbar( World* world ) : MiddleAreaPage( world, "Toolbar" ) {}

	void Create();
	void Destroy();
	void Align();
	
private:
	
};

}
}
}
