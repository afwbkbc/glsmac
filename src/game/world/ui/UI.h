#pragma once

#include "ui/object/UIContainer.h"

namespace game {
namespace world {
	
class World;
	
namespace ui {
	
CLASS( UI, UIContainer )
	
	UI( const World* world, const std::string& class_name );
	
protected:
	const World* m_world = nullptr;

};

}
}
}
