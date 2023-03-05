#pragma once

#include "ui/object/UIContainer.h"

namespace game {
namespace world {
	
class World;
	
namespace ui {
	
CLASS( UI, UIContainer )
	
	UI( World* world, const std::string& class_name );
	
protected:
	World* m_world = nullptr;

};

}
}
}
